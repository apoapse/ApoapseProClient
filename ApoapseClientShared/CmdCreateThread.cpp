#include "stdafx.h"
#include "CmdCreateThread.h"
#include "Common.h"
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
		Field{ "uuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Uuid::IsValid) },
		Field{ "room_uuid", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(ByteContainer, Uuid::IsValid) },
	};

	info.metadataTypes = MetadataAcess::usergroup;
	info.metadataSelfFields =
	{
		Field{ "name", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, [&](const auto& str) { return !str.empty(); }) },
	};

	return info;
}

void CmdCreateThread::Process(ClientConnection& sender)
{
	auto& metadataUserGrp = GetMetadataField(MetadataAcess::usergroup);
	auto& usergroupData = metadataUserGrp.ReadData();

	const auto uuid = Uuid(GetFieldsData().GetValue<ByteContainer>("uuid"));
	const auto roomUuid = Uuid(GetFieldsData().GetValue<ByteContainer>("room_uuid"));

	//sender.client.GetRoomManager().AddNewThreadFromServer(uuid, roomUuid, usergroupData.GetValue<std::string>("name"));
}

void CmdCreateThread::SendCreateThread(const Uuid& threadUuid, const Uuid& roomUuid, const std::string& name, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("uuid", threadUuid.GetInRawFormat());
	ser.UnorderedAppend("room_uuid", roomUuid.GetInRawFormat());

	{
		MessagePackSerializer serMetadata;
		serMetadata.UnorderedAppend("name", name);
		ser.UnorderedAppend("metadata_usergroup", ApoapseMetadata(serMetadata, MetadataAcess::usergroup).GetRawData());
	}

	CmdCreateThread cmd;
	cmd.Send(ser, *client.GetConnection());
}

APOAPSE_COMMAND_REGISTER(CmdCreateThread, CommandId::create_thread);