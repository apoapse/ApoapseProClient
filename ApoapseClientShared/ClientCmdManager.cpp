#include "stdafx.h"
#include "Common.h"
#include "ClientCmdManager.h"
#include "ClientConnection.h"
#include "CommandsDef.hpp"
#include "LocalUser.h"
#include "ApoapseClient.h"

ClientCmdManager::ClientCmdManager() : CommandsManagerV2(GetCommandDef())
{
}

bool ClientCmdManager::OnSendCommandPre(CommandV2& cmd)
{
	if (cmd.name == "install")
	{
		const auto username = User::HashUsername(cmd.GetData().GetField("admin_username").GetValue<std::string>());
		const auto password = User::HashPasswordForServer(cmd.GetData().GetField("admin_password").GetValue<std::string>());

		cmd.GetData().GetField("admin_username").SetValue(username);
		cmd.GetData().GetField("admin_password").SetValue(password);
	}

	return true;
}

bool ClientCmdManager::OnReceivedCommandPre(CommandV2& cmd, GenericConnection& netConnection)
{
	return true;
}

void ClientCmdManager::OnReceivedCommand(CommandV2& cmd, GenericConnection& netConnection)
{
	auto& connection = static_cast<ClientConnection&>(netConnection);

	if (cmd.name == "server_info")
	{
		const auto status = cmd.GetData().GetField("status").GetValue<std::string>();

		if (status == "authenticated")
		{
			if (!cmd.GetData().GetField("requirePasswordChange").GetValue<bool>())
			{
				User user;
				user.nickname = cmd.GetData().GetField("nickname").GetValue<std::string>();
				user.username = cmd.GetData().GetField("username").GetValue<Username>();

				connection.client.Authenticate(user);
			}
		}
	}
	
	else if (cmd.name == "create_room")
	{

	}

	LOG_DEBUG << "RECEIVED!";
}
