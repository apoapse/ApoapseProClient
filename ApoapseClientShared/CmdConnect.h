#pragma once
#include "Command.h"
#include "MessagePack.hpp"
#include "CryptographyTypes.hpp"
#include "Username.h"
class ClientConnection;

class CmdConnect final : public Command
{


public:
	CommandInfo& GetInfo() const override;

	void PrepareLoginCmd(const Username& username, const std::string& password);	// Prepare the whole command content with password and username encryption

private:

};