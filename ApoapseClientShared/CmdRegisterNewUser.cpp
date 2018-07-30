#include "stdafx.h"
#include "CmdRegisterNewUser.h"
#include "Common.h"
#include "ApoapseClient.h"
#include "ClientConnection.h"
#include "User.h"

CommandInfo& CmdRegisterNewUser::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::register_new_user;
	info.clientOnly = true;
	//info.requireAuthentication = true;	#MVP active

	return info;
}

void CmdRegisterNewUser::SendRegisterCommand(const Username& username, const std::vector<byte>& password, ApoapseClient& client)
{
 	MessagePackSerializer ser;
	ser.UnorderedAppend("username", username.GetRaw());
	//ser.UnorderedAppend("temporary_password", User::GenerateTemporaryPassword()); // #MVP to re-enable and remove password function arg
	ser.UnorderedAppend("temporary_password", password);

	CmdRegisterNewUser cmd;
	cmd.Send(ser, *client.GetConnection());
}
