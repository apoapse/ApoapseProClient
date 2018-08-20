#include "stdafx.h"
#include "CmdSyncRequest.h"
#include "Common.h"
#include "ApoapseClient.h"
#include "ClientConnection.h"
#include "User.h"

CommandInfo& CmdSyncRequest::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::sync_request;
	info.serverOnly = true;

	return info;
}

void CmdSyncRequest::SendSyncRequest(Int64 mostRecentOperationTime, ApoapseClient& client)
{
	MessagePackSerializer ser;
	ser.UnorderedAppend("sinceTimestamp", mostRecentOperationTime);

	CmdSyncRequest cmd;
	cmd.Send(ser, *client.GetConnection());
}
