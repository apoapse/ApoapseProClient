#pragma once
#include "Command.h"
#include "Uuid.h"
#include "Username.h"
#include "DateTimeUtils.h"
class ApoapseClient;
class ApoapseThread;
class ApoapseMessage;

class CmdApoapseMessage final : public Command
{


public:
	CommandInfo& GetInfo() const override;
	void Process(ClientConnection& sender) override;

	static void SendMessage(const ApoapseMessage& message, ApoapseThread& thread, ApoapseClient& client);
private:

};