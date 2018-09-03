#pragma once
#include "Command.h"
#include "Username.h"
class ApoapseClient;

class CmdMarkMessageAsRead final : public Command
{

public:
	CommandInfo& GetInfo() const override;
	void Process(ClientConnection& sender) override;

	static void SendMarkMessageAsRead(const Uuid& messageUuid, ApoapseClient& client);
private:

};