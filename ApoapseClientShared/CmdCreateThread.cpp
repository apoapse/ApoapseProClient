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
		CommandField{ "room_uuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(ByteContainer, Uuid::IsValid) },
	};

	return info;
}

void CmdCreateThread::Process(ClientConnection& sender)
{
	const auto uuid = Uuid(GetFieldsData().GetValue<ByteContainer>("uuid"));
	const auto roomUuid = Uuid(GetFieldsData().GetValue<ByteContainer>("room_uuid"));

	sender.client.GetRoomManager().AddNewThreadFromServer(uuid, roomUuid, ""); // #MVP support names
}

void CmdCreateThread::SendCreateThread(const Uuid& threadUuid, const Uuid& roomUuid, const std::string& name, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("uuid", threadUuid.GetInRawFormat());
	ser.UnorderedAppend("room_uuid", roomUuid.GetInRawFormat());

	CmdCreateThread cmd;
	cmd.Send(ser, *client.GetConnection());
}

APOAPSE_COMMAND_REGISTER(CmdCreateThread, CommandId::create_thread);