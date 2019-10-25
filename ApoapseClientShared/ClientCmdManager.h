#pragma once
#include "TypeDefs.hpp"
#include "CommandsManagerV2.h"
class ApoapseClient;

class ClientCmdManager : public CommandsManagerV2
{
	ApoapseClient& apoapseClient;
	bool m_isSynchronizing = false;
	Int64 m_itemsToSyncTotal = 0;
	Int64 m_itemsSynced = 0;
	
public:
	
	ClientCmdManager(ApoapseClient& client);

	// Inherited via CommandsManagerV2
	bool OnSendCommandPre(CommandV2& cmd) override;
	bool OnReceivedCommandPre(CommandV2& cmd, GenericConnection& netConnection) override;
	void OnReceivedCommand(CommandV2& cmd, GenericConnection& netConnection) override;
	void OnReceivedCommandPost(CommandV2& cmd, GenericConnection& netConnection) override;

	bool IsSynchronizing() const;
	void Reset();
	JsonHelper GetSyncUIJson() const;
};