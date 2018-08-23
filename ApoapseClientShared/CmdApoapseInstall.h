#pragma once
#include "Command.h"
#include "Username.h"
class ApoapseClient;

class CmdApoapseInstall final : public Command
{

public:
	CommandInfo& GetInfo() const override;
	static void SendInstallCommand(const Username& username, const std::vector<byte>& password, const std::string& nickname, ApoapseClient& client);

private:

};