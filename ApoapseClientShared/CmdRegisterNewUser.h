#pragma once
#include "Command.h"
#include "Username.h"
class ApoapseClient;

class CmdRegisterNewUser : public Command
{
	
public:
	CommandInfo& GetInfo() const override;
	static void SendRegisterCommand(const Username& username, const std::vector<byte>& password, ApoapseClient& client);

private:

};