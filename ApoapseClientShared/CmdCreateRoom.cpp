#include "stdafx.h"
#include "CmdCreateRoom.h"
#include "Common.h"
#include "RoomManager.h"
#include "ClientConnection.h"
#include "ApoapseClient.h"
#include "CommandsManager.h"
#include "Operation.h"

CommandInfo& CmdCreateRoom::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::create_room;
	info.requireAuthentication = true;
	info.fields =
	{
		CommandField{ "uuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Uuid::IsValid) },
	};

	return info;
}

void CmdCreateRoom::Process(ClientConnection& sender)
{
	auto room = std::make_unique<ApoapseRoom>();
	room->uuid = Uuid(GetFieldsData().GetValue<ByteContainer>("uuid"));

	sender.client.GetRoomManager().AddNewRoomFromServer(std::move(room));
}

void CmdCreateRoom::SendCreateRoom(const ApoapseRoom& room, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("uuid", room.uuid.GetInRawFormat());

	CmdCreateRoom cmd;
	cmd.Send(ser, *client.GetConnection());
}

APOAPSE_COMMAND_REGISTER(CmdCreateRoom, CommandId::create_room);