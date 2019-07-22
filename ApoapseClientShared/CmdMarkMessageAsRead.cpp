#include "stdafx.h"
#include "CmdCreateThread.h"
#include "Common.h"
#include "ClientConnection.h"
#include "ApoapseClient.h"
#include "CommandsManager.h"
#include "CmdMarkMessageAsRead.h"

CommandInfo& CmdMarkMessageAsRead::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::mark_message_as_read;
	info.requireAuthentication = true;
	info.fields =
	{
		Field{ "messageUuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Uuid::IsValid) },
	};

	return info;
}

void CmdMarkMessageAsRead::SendMarkMessageAsRead(const Uuid& messageUuid, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("messageUuid", messageUuid.GetInRawFormat());

	CmdMarkMessageAsRead cmd;
	cmd.Send(ser, *client.GetConnection());
}

void CmdMarkMessageAsRead::Process(ClientConnection& sender)
{
	const auto msgUuid = Uuid(GetFieldsData().GetValue<ByteContainer>("messageUuid"));

	//ApoapseMessage::MarkMessageAsReadFromServer(msgUuid, sender.client);
}

APOAPSE_COMMAND_REGISTER(CmdMarkMessageAsRead, CommandId::mark_message_as_read);