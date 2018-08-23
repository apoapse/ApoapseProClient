#include "stdafx.h"
#include "CmdApoapseInstall.h"
#include "Common.h"
#include "ApoapseClient.h"
#include "ClientConnection.h"
#include "User.h"

CommandInfo& CmdApoapseInstall::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::apoapse_install;
	info.serverOnly = true;
	info.metadataTypes = MetadataAcess::all;

	return info;
}

void CmdApoapseInstall::SendInstallCommand(const Username& username, const std::vector<byte>& password, const std::string& nickname, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("admin_username", username.GetRaw());
	ser.UnorderedAppend("admin_password", password);

	{
		MessagePackSerializer serMetadata;
		serMetadata.UnorderedAppend("nickname", nickname);
		ser.UnorderedAppend("metadata_all", ApoapseMetadata(serMetadata, MetadataAcess::all).GetRawData());
	}

	CmdApoapseInstall cmd;
	cmd.Send(ser, *client.GetConnection());
}
