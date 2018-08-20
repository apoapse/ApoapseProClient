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

}

JsonHelper ApoapseMessage::GenerateJson(Int64 internalId) const
{
	JsonHelper serMessage;

	serMessage.Insert("internal_id", internalId);
	serMessage.Insert("sent_time", sentTime.str());
	serMessage.Insert("author", HTMLUI::HtmlSpecialChars(author.ToStr(), false));
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

		SQLQuery query(*global->database);
		query << INSERT_INTO << "messages" << " (id, uuid, thread_uuid, author, sent_time, content)"
			<< VALUES << "(" << message->dbId << "," << message->uuid.GetInRawFormat() << "," << message->thread.uuid.GetInRawFormat() << "," << message->author.GetRaw() << "," << message->sentTime.str() << "," << formatedContent << ")";
		query.Exec();

		Operation(OperationType::new_message, roomManager.apoapseClient.GetLocalUser().username, message->dbId).Save();
	}

	{
		auto* activeThread = roomManager.GetActiveThread();
		if (activeThread != nullptr && activeThread->uuid == message->thread.uuid)
		{
			activeThread->OnAddedNewMessageFromServer(std::move(message));
		}
	}

	ApoapseThread::UpdateThreadLastMessagePreview(*threadPtr, roomManager);
}

bool ApoapseMessage::DoesMessageExist(const Uuid& uuid)
{
	SQLQuery query(*global->database);
	query << "SELECT Count(*) FROM messages WHERE uuid " << EQUALS << uuid.GetInRawFormat();
	auto res = query.Exec();

	return (res[0][0].GetInt64() == 1);
}
