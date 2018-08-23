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
	info.metadataTypes = MetadataAcess::self;
	info.fields =
	{
		Field{ "uuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Uuid::IsValid) },
	};

	info.metadataSelfFields =
	{
		Field{ "name", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, [&](const auto& str) { return !str.empty(); }) },
	};

	return info;
}

void CmdCreateRoom::Process(ClientConnection& sender)
{
	auto& metadataSelf = GetMetadataField(MetadataAcess::self);
	auto& selfData = metadataSelf.ReadData();

	auto room = std::make_unique<ApoapseRoom>();
	room->uuid = Uuid(GetFieldsData().GetValue<ByteContainer>("uuid"));
	room->name = selfData.GetValue<std::string>("name");

	sender.client.GetRoomManager().AddNewRoomFromServer(std::move(room));
}

void CmdCreateRoom::SendCreateRoom(const ApoapseRoom& room, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("uuid", room.uuid.GetInRawFormat());

	{
		MessagePackSerializer serMetadata;
		serMetadata.UnorderedAppend("name", room.name);
		ser.UnorderedAppend("metadata_self", ApoapseMetadata(serMetadata, MetadataAcess::self).GetRawData());
	}

	CmdCreateRoom cmd;
	cmd.Send(ser, *client.GetConnection());
}

APOAPSE_COMMAND_REGISTER(CmdCreateRoom, CommandId::create_room);