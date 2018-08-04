#pragma once
#pragma once
#include "Command.h"
#include "MessagePack.hpp"
#include "CryptographyTypes.hpp"
#include "Username.h"
struct ApoapseRoom;
class ApoapseClient;

class CmdCreateRoom final : public Command
{


public:
	CommandInfo& GetInfo() const override;
	void Process(ClientConnection& sender) override;

	static void SendCreateRoom(const ApoapseRoom& room, ApoapseClient& client);
private:

};