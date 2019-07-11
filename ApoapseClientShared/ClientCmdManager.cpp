#include "stdafx.h"
#include "Common.h"
#include "ClientCmdManager.h"
#include "ClientConnection.h"
#include "CommandsDef.hpp"

ClientCmdManager::ClientCmdManager() : CommandsManagerV2(GetCommandDef())
{
}

bool ClientCmdManager::OnReceivedCommandPre(CommandV2& cmd, GenericConnection& netConnection)
{
	return false;
}

void ClientCmdManager::OnReceivedCommand(CommandV2& cmd, GenericConnection& netConnection)
{
	auto& connection = static_cast<ClientConnection&>(netConnection);

	LOG_DEBUG << "RECEIVED!";
}
