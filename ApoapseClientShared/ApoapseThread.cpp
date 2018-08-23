#include "stdafx.h"
#include "ApoapseThread.h"
#include "Common.h"
#include "SQLQuery.h"
#include "DateTimeUtils.h"
#include "HTMLUI.h"
#include "CmdApoapseMessage.h"
#include "RoomManager.h"
#include "ApoapseClient.h"
#include "LocalUser.h"
#include "SQLUtils.hpp"

ApoapseThread::ApoapseThread(const SimpleApoapseThread& simpleThread, RoomManager* roomManager) : roomManager(roomManager)
{
	dbId = simpleThread.dbId;
	uuid = simpleThread.uuid;
	roomUuid = simpleThread.roomUuid;
	name = simpleThread.name;
	lastMessageAuthor = simpleThread.lastMessageAuthor;
	lastMessageText = simpleThread.lastMessageText;
}

void ApoapseThread::OnUIDisplay()
{
	UpdateMessagesListUI();
}

void ApoapseThread::SendNewMessage(const std::string& content)
{
	ApoapseMessage messageObj(*this);
	messageObj.uuid = Uuid::Generate();
	messageObj.sentTime = DateTimeUtils::UTCDateTime::CurrentTime();
	messageObj.author = roomManager->apoapseClient.GetLocalUser().username;
	messageObj.content = content;

	CmdApoapseMessage::SendMessage(messageObj, *this, roomManager->apoapseClient);
}

void ApoapseThread::OnAddedNewMessageFromServer(std::unique_ptr<ApoapseMessage> message)
{
	const Int64 idOnUI = m_messages.size();
	m_messages.push_back(std::move(message));

	{
		auto& savedMsg = m_messages[m_messages.size() - 1];
		global->htmlUI->SendSignal("added_new_message", savedMsg->GenerateJson(idOnUI).Generate());
	}
}

ApoapseMessage* ApoapseThread::GetMostRecentMessage() const
{
	if (m_messages.size() == 0)
		return nullptr;

	return (&*m_messages[m_messages.size() - 1]);
}

void ApoapseThread::UpdateThreadLastMessagePreview(SimpleApoapseThread& thread, RoomManager& roomManager) // #TODO This function is not aware of any messages cache and do not use the Message class utils to get the data
{
	auto* activeThread = roomManager.GetActiveThread();
	std::string messageContent;
	Username author;

	if (activeThread != nullptr && activeThread->uuid == thread.uuid)
	{
		const auto* lastMessage = activeThread->GetMostRecentMessage();

		if (lastMessage != nullptr)
		{
			messageContent = lastMessage->content;
			author = lastMessage->author;
		}
	}
	else
	{
		SQLQuery query(*global->database);
		query << SELECT << "author, content" << FROM << "messages" << WHERE << "thread_uuid" << EQUALS << thread.uuid.GetInRawFormat() << ORDER_BY << "id" << DESC << LIMIT << 1;
		auto res = query.Exec();

		if (res.RowCount() == 1)
		{
			author = Username(res[0][0].GetByteArray());
			ByteContainer unformatedContent = res[0][1].GetByteArray();
			messageContent = std::string(unformatedContent.begin(), unformatedContent.end());
		}
	}

	if (!messageContent.empty())
	{
		thread.lastMessageAuthor = author;
		thread.lastMessageText = messageContent;

		if (roomManager.GetSelectedRoom()->uuid == thread.roomUuid)
		{
			UpdateUIThreadMsgPreview(thread);
		}
	}
}

void ApoapseThread::UpdateUIThreadMsgPreview(const SimpleApoapseThread& thread)
{
	JsonHelper ser;
	ser.Insert("dbid", thread.dbId);
	ser.Insert("lastMsgAuthor", User::GetUserByUsername(thread.lastMessageAuthor).nickname);
	ser.Insert("lastMsgText", HTMLUI::HtmlSpecialChars(thread.lastMessageText, false));

	global->htmlUI->SendSignal("updateThreadMsgPreview", ser.Generate());
}

void ApoapseThread::LoadMessages()
{
	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "messages" << WHERE << "thread_uuid" << EQUALS << uuid.GetInRawFormat() << ORDER_BY << "id" << DESC;
	auto res = query.Exec();

	for (const auto& row : res)
	{
		auto message = std::make_unique<ApoapseMessage>(*this);
		message->dbId = row[0].GetInt64();
		message->uuid = Uuid(row[1].GetByteArray());
		message->author = Username(row[3].GetByteArray());
		message->sentTime = DateTimeUtils::UTCDateTime(row[4].GetText());
		message->thread = *this;

		{
			const auto contentRaw = row[5].GetByteArray();
			message->content = std::string(contentRaw.begin(), contentRaw.end());
		}
		
		m_messages.push_back(std::move(message));
	}
}

void ApoapseThread::UpdateMessagesListUI() const
{
	JsonHelper ser;

	// Reverse loop
	for (size_t i = m_messages.size(); i-- > 0;)
	{
		const auto& message = m_messages[i];

		ser.Insert("messages", message->GenerateJson(i));
	}

	global->htmlUI->SendSignal("open_thread", ser.Generate());
}
