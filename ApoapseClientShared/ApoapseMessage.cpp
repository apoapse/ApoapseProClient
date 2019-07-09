#include "stdafx.h"
#include "ApoapseMessage.h"
#include "Common.h"
#include "ApoapseThread.h"
#include "RoomManager.h"
#include "SQLUtils.hpp"
#include "HTMLUI.h"
#include "Operation.h"
#include "ApoapseClient.h"

ApoapseMessage::ApoapseMessage(SimpleApoapseThread& thread) : thread(thread)
{
	ASSERT(thread.uiId >= 0);
}

void ApoapseMessage::MarkMessageAsReadFromServer(const Uuid& uuid, ApoapseClient& client)
{
	if (!DoesMessageExist(uuid))
	{
		LOG << LogSeverity::error << "Trying to mark a message as read that does not exist: " << uuid.GetBytes();
		return;
	}

	// #TODO Make sure the message is not already marked as read
	{
		SQLQuery query(*global->database);
		query << SELECT << "is_read" << FROM << "messages" << WHERE << "uuid" << EQUALS << uuid.GetInRawFormat();
		auto res = query.Exec();

		const bool isAlreadyRead = res[0][0].GetBoolean();

		if (isAlreadyRead)
		{
			LOG << LogSeverity::error << "Trying to mark a message as read that is already read: " << uuid.GetBytes();
			return;
		}
	}

	ApoapseMessage message = ApoapseMessage::GetMessageByUuid(uuid, client.GetRoomManager());

	{
		SQLQuery query(*global->database);
		query << UPDATE << "messages" << SET << "is_read=" << 1 << WHERE "uuid" << EQUALS << uuid.GetInRawFormat();
		query.Exec();
		
		Operation(OperationType::mark_message_as_read, client.GetLocalUser().username, message.dbId).Save();
	}

	client.GetRoomManager().OnMarkMessageAsRead(message.thread);

	message.OnChangedReadStatus(ReadStatus::marked_as_read, client.GetRoomManager());
}

ApoapseMessage ApoapseMessage::GetMessageByUuid(const Uuid& uuid, RoomManager& roomManager)
{
	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "messages" << WHERE << "uuid" << EQUALS << uuid.GetInRawFormat();
	auto res = query.Exec();	// #TODO #MVP use a generic system that can generate a Message obj from a SQLRow

	const auto threadUuid = Uuid(res[0][2].GetByteArray());
	auto* relatedThread = roomManager.GetThreadByUuid(threadUuid);

	if (relatedThread == nullptr)
		throw std::exception("The thread related to the request message does not exist");

	ApoapseMessage message(*relatedThread);
	message.dbId = res[0][0].GetInt64();
	message.uuid = uuid;
	message.author = Username(res[0][3].GetByteArray());
	message.sentTime = DateTimeUtils::UTCDateTime(res[0][4].GetText());
	message.content = res[0][4].GetText();
	message.isRead = static_cast<bool>(res[0][6].GetInt32());

	return message;
}

void ApoapseMessage::OnChangedReadStatus(ReadStatus readStatus, RoomManager& roomManager) const
{
	const std::string status = (readStatus == ReadStatus::marked_as_read) ? "marked_as_read" : "marked_as_unread";

	JsonHelper ser;
	ser.Insert("status", status);
	ser.Insert("messageDbId", dbId);
	ser.Insert("threadDbId", thread.dbId);
	ser.Insert("roomUiId", roomManager.GetRoomByUuid(thread.roomUuid)->uiId);
	ser.Insert("roomDbId", roomManager.GetRoomByUuid(thread.roomUuid)->dbId);
	ser.Insert("threadUnreadMsgCount", thread.unreadMessagesCount);
	ser.Insert("roomUnreadMsgCount", roomManager.GetRoomByUuid(thread.roomUuid)->unreadMessagesCount);

	global->htmlUI->SendSignal("UpdateUnreadMessagesCount", ser.Generate());
}

JsonHelper ApoapseMessage::GenerateJson(Int64 internalId) const
{
	JsonHelper serMessage;
	
	serMessage.Insert("internal_id", internalId);
	serMessage.Insert("dbid", dbId);
	serMessage.Insert("sent_time", sentTime.GetStr());
	serMessage.Insert("author", HTMLUI::HtmlSpecialChars(User::GetUserByUsername(author).nickname, false));
	serMessage.Insert<bool>("isRead", isRead);
	serMessage.Insert("content", HTMLUI::HtmlSpecialChars(content, true));

	return serMessage;
}

void ApoapseMessage::AddNewMessageFromServer(std::unique_ptr<ApoapseMessage> message, RoomManager& roomManager)
{
	SimpleApoapseThread* threadPtr = &message->thread;	// Used so that we can keep access to the object after it has been moved

	if (DoesMessageExist(message->uuid))
	{
		LOG << LogSeverity::error << "Trying to add a new message that is already here";
		return;
	}

	// Save
	{
		message->dbId = SQLUtils::CountRows("messages");
		std::vector<byte> formatedContent(message->content.begin(), message->content.end());
		const int isRead = (message->author == roomManager.apoapseClient.GetLocalUser().username) ? 1 : 0;
		message->isRead = (bool)isRead;

		SQLQuery query(*global->database);
		query << INSERT_INTO << "messages" << " (id, uuid, thread_uuid, author, sent_time, content, is_read)"
			<< VALUES << "(" << message->dbId << "," << message->uuid.GetInRawFormat() << "," << message->thread.uuid.GetInRawFormat() << "," << message->author.GetRaw() << ","
			<< message->sentTime.GetStr() << "," << formatedContent << "," << isRead << ")";
		query.Exec();

		Operation(OperationType::new_message, roomManager.apoapseClient.GetLocalUser().username, message->dbId).Save();
	}

	{
		if (!message->isRead)
		{
			roomManager.OnAddedUnreadMessage(message->thread);

			message->OnChangedReadStatus(ReadStatus::marked_as_unread, roomManager);
		}

		auto* activeThread = roomManager.GetActiveThread();
		if (activeThread != nullptr && activeThread->uuid == message->thread.uuid)
		{
			activeThread->OnAddedNewMessageFromServer(std::move(message));
		}
	}

	LOG << "Added new Apoapse Message";

	ApoapseThread::UpdateThreadLastMessagePreview(*threadPtr, roomManager);
}

bool ApoapseMessage::DoesMessageExist(const Uuid& uuid)
{
	SQLQuery query(*global->database);
	query << "SELECT Count(*) FROM messages WHERE uuid " << EQUALS << uuid.GetInRawFormat();
	auto res = query.Exec();

	return (res[0][0].GetInt64() == 1);
}
