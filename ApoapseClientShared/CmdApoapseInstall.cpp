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


	return info;
}

void CmdApoapseInstall::SendInstallCommand(const Username& username, const std::vector<byte>& password, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("admin_username", username.GetRaw());
	ser.UnorderedAppend("admin_password", password);

	CmdApoapseInstall cmd;
	cmd.Send(ser, *client.GetConnection());
}
