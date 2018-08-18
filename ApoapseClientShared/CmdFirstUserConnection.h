#pragma once
#include "Command.h"
#include "Username.h"
class ApoapseClient;

class CmdFirstUserConnection final : public Command
{

public:
	CommandInfo& GetInfo() const override;
	static void SetUserIdentity(const std::vector<byte>& password, ApoapseClient& client);

private:

};