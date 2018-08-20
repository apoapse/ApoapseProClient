#pragma once
#include "Command.h"
#include "Username.h"
class ApoapseClient;

class CmdSyncRequest final : public Command
{

public:
	CommandInfo& GetInfo() const override;
	static void SendSyncRequest(Int64 mostRecentOperationTime, ApoapseClient& client);

private:

};