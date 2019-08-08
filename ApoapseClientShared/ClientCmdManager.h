#pragma once
#include "TypeDefs.hpp"
#include "CommandsManagerV2.h"
class ApoapseClient;

class ClientCmdManager : public CommandsManagerV2
{
	ApoapseClient& apoapseClient;

public:
	ClientCmdManager(ApoapseClient& client);

	// Inherited via CommandsManagerV2
	virtual bool OnSendCommandPre(CommandV2& cmd) override;
	virtual bool OnReceivedCommandPre(CommandV2& cmd, GenericConnection& netConnection) override;
	virtual void OnReceivedCommand(CommandV2& cmd, GenericConnection& netConnection) override;
	virtual void OnReceivedCommandPost(CommandV2& cmd, GenericConnection& netConnection) override;
};