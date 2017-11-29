#pragma once
#include "Command.h"
#include "MessagePack.hpp"
#include "CryptographyTypes.hpp"
class ClientConnection;

class CmdConnect : public Command
{


public:
	CommandInfo& GetInfo() const override;

	void PrepareLoginCmd(const std::string& username, const std::string& password);	// Prepare the whole command content with password and username encryption

private:

};