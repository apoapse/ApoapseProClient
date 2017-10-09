#pragma once
#include "Command.h"
#include "MessagePack.hpp"
#include "HashTypes.hpp"
class ClientConnection;

class CmdConnect : public Command
{
	static constexpr UInt32 passwordHashIterations = 500'000;

public:
	CommandInfo& GetInfo() const override;

	void PrepareLoginCmd(const std::string& username, const std::string& password);	// Prepare the whole command content with password and username encryption

private:
	static std::vector<byte> HashUsername(const std::string& username);
	static std::vector<byte> HashPassword(const std::string& password);
};