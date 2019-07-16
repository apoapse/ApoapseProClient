#pragma once
#include "TypeDefs.hpp"
#include "CommandsManagerV2.h"

class ClientCmdManager : public CommandsManagerV2
{
public:
	ClientCmdManager();

	// Inherited via CommandsManagerV2
	virtual bool OnSendCommandPre(CommandV2& cmd) override;
	virtual bool OnReceivedCommandPre(CommandV2& cmd, GenericConnection& netConnection) override;
	virtual void OnReceivedCommand(CommandV2& cmd, GenericConnection& netConnection) override;
};