#include "stdafx.h"
#include "CmdFirstUserConnection.h"
#include "Common.h"
#include "ApoapseClient.h"
#include "ClientConnection.h"
#include "User.h"

CommandInfo& CmdFirstUserConnection::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::first_user_connection;
	info.serverOnly = true;
	info.metadataTypes = MetadataAcess::all;

	return info;
}

void CmdFirstUserConnection::SetUserIdentity(const std::vector<byte>& password, const std::string& nickname, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("password", password);

	{
		MessagePackSerializer serMetadata;
		serMetadata.UnorderedAppend("nickname", nickname);
		ser.UnorderedAppend("metadata_all", ApoapseMetadata(serMetadata, MetadataAcess::all).GetRawData());
	}

	CmdFirstUserConnection cmd;
	cmd.Send(ser, *client.GetConnection());
}