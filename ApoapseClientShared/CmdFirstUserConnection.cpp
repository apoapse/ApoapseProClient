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


	return info;
}

void CmdFirstUserConnection::SetUserIdentity(const std::vector<byte>& password, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("password", password);

	CmdFirstUserConnection cmd;
	cmd.Send(ser, *client.GetConnection());
}