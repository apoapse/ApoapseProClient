#include "stdafx.h"
#include "CmdApoapseMessage.h"
#include "Common.h"
#include "RoomManager.h"
#include "ApoapseThread.h"
#include "ClientConnection.h"
#include "ApoapseClient.h"
#include "CommandsManager.h"
#include "DateTimeUtils.h"
#include "Username.h"

CommandInfo& CmdApoapseMessage::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::apoapse_message;
	info.requireAuthentication = true;
	info.fields =
	{
		Field{ "uuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Uuid::IsValid) },
		Field{ "threadUuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Uuid::IsValid) },
		Field{ "sentTime", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, DateTimeUtils::UTCDateTime::ValidateFormat) },
		Field{ "author", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(ByteContainer, Username::IsValid) },
		Field{ "content", FieldRequirement::any_mendatory, FIELD_VALUE(std::vector<byte>) },
	};

	return info;
}

void CmdApoapseMessage::Process(ClientConnection& sender)
{
	const auto formatedContent = GetFieldsData().GetValue<ByteContainer>("content");
	const auto threadUuid = Uuid(GetFieldsData().GetValue<ByteContainer>("threadUuid"));

	auto* relatedThread = sender.client.GetRoomManager().GetThreadByUuid(threadUuid);
	if (relatedThread == nullptr)
	{
		LOG << LogSeverity::error << "The thread " << threadUuid.GetAsByteVector() << " where this message is stored does not exist";
		return;
	}

	auto message = std::make_unique<ApoapseMessage>(*relatedThread);
	message->uuid = Uuid(GetFieldsData().GetValue<ByteContainer>("uuid"));
	message->sentTime = DateTimeUtils::UTCDateTime(GetFieldsData().GetValue<std::string>("sentTime"));
	message->author = Username(GetFieldsData().GetValue<ByteContainer>("author"));	// #MVP #SECURITY make sure the author exist
	message->content = std::string(formatedContent.begin(), formatedContent.end());
	message->thread = *relatedThread;

	ApoapseMessage::AddNewMessageFromServer(std::move(message), sender.client.GetRoomManager());

// 	if (message.sentTime > DateTimeUtils::UTCDateTime::CurrentTime())
// 	{
// 		LOG << LogSeverity::error << "The message sent time is not valid";
// 		return;
// 	}
}

void CmdApoapseMessage::SendMessage(const ApoapseMessage& message, ApoapseThread& thread, ApoapseClient& client)
{
	const auto formatedContent = std::vector<byte>(message.content.begin(), message.content.end());

	MessagePackSerializer ser;
	ser.UnorderedAppend("uuid", message.uuid.GetInRawFormat());
	ser.UnorderedAppend("threadUuid", thread.uuid.GetInRawFormat());
	
	ser.UnorderedAppend("sentTime", message.sentTime.str());
	ser.UnorderedAppend("content", formatedContent);

	CmdApoapseMessage cmd;
	cmd.Send(ser, *client.GetConnection());
}

APOAPSE_COMMAND_REGISTER(CmdApoapseMessage, CommandId::apoapse_message);