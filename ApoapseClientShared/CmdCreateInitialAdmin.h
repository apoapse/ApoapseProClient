#pragma once
#include "Command.h"
class ApoapseClient;

class CmdCreateInitialAdmin : public Command
{

public:
	CommandInfo& GetInfo() const override;

	static void CreateAndSend(const std::string& username, const std::string& password, ApoapseClient& client);
};