#pragma once
#include "Command.h"
#include "Uuid.h"
class ApoapseClient;
struct ApoapseRoom;

class CmdCreateThread final : public Command
{


public:
	CommandInfo& GetInfo() const override;
	void Process(ClientConnection& sender) override;

	static void SendCreateThread(const Uuid& threadUuid, const Uuid& roomUuid, const std::string& name, ApoapseClient& client);
private:

};