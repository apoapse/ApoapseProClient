#include "stdafx.h"
#include "CmdCreateThread.h"
#include "Common.h"
#include "RoomManager.h"
#include "ClientConnection.h"
#include "ApoapseClient.h"
#include "CommandsManager.h"

CommandInfo& CmdCreateThread::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::create_thread;
	info.requireAuthentication = true;
	info.fields =
	{
		CommandField{ "uuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Uuid::IsValid) },
	};

	return info;
}

void CmdCreateThread::Process(ClientConnection& sender)
{
	const auto uuid = Uuid(GetFieldsData().GetValue<ByteContainer>("uuid"));

	sender.client.GetRoomManager().AddNewThreadFromServer(uuid, ""); // #MVP support names
}

void CmdCreateThread::SendCreateThread(const Uuid& uuid, const std::string& name, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("uuid", uuid.GetInRawFormat());

	CmdCreateThread cmd;
	cmd.Send(ser, *client.GetConnection());
}

APOAPSE_COMMAND_REGISTER(CmdCreateThread, CommandId::create_thread);