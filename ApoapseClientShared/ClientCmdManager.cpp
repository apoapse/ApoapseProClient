#include "stdafx.h"
#include "Common.h"
#include "ClientCmdManager.h"
#include "ClientConnection.h"
#include "CommandsDef.hpp"
#include "LocalUser.h"
#include "ApoapseClient.h"
#include "HTMLUI.h"

ClientCmdManager::ClientCmdManager(ApoapseClient& client) : CommandsManagerV2(GetCommandDef()), apoapseClient(client)
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
	
	else if (cmd.name == "create_thread")
	{
		cmd.GetData().GetField("parent_room").SetValue(apoapseClient.GetContentManager().GetCurrentRoom().uuid);
	}

	else if (cmd.name == "new_message")
	{
		cmd.GetData().GetField("parent_thread").SetValue(apoapseClient.GetContentManager().GetCurrentThread().uuid);
		cmd.GetData().GetField("sent_time").SetValue(DateTimeUtils::UTCDateTime::CurrentTime());
		cmd.GetData().GetField("author").SetValue(apoapseClient.GetLocalUser().username);
	}

	return true;
}

bool ClientCmdManager::OnReceivedCommandPre(CommandV2& cmd, GenericConnection& netConnection)
{
	// Checks if the operation/item is not already registered
	if (cmd.operationRegister)
	{
		const Uuid operationUuid = cmd.GetData().GetField("operation_uuid").GetValue<Uuid>();

		if (!apoapseClient.GetClientOperations().IsAlreadyRegistered(operationUuid))
		{
			apoapseClient.GetClientOperations().RegisterOperationUuid(operationUuid);
		}
		else
		{
			LOG << LogSeverity::error << "Rejecting the command " << cmd.name << " because the operation " << operationUuid.GetBytes() << " is aready registered";
			return false;
		}
	}

	return true;
}

void ClientCmdManager::OnReceivedCommand(CommandV2& cmd, GenericConnection& netConnection)
{
	auto& connection = dynamic_cast<ClientConnection&>(netConnection);

	if (cmd.name == "server_info")
	{
		const auto status = cmd.GetData().GetField("status").GetValue<std::string>();

		if (status == "authenticated")
		{
			if (!cmd.GetData().GetField("requirePasswordChange").GetValue<bool>())
			{
				auto usergroupsDat = cmd.GetData().GetField("usergroups").GetDataArray();
				apoapseClient.InitUsergroupManager(usergroupsDat);

				LocalUser user;
				user.nickname = HTMLUI::HtmlSpecialChars(cmd.GetData().GetField("nickname").GetValue<std::string>(), true);
				user.username = cmd.GetData().GetField("username").GetValue<Username>();
				user.usergroup = &apoapseClient.GetUsergroupManager().GetUsergroup(cmd.GetData().GetField("usergroup").GetValue<Uuid>());

				apoapseClient.Authenticate(user);
			}
		}
	}

	if (cmd.name == "user")
	{
		auto user = User(cmd.GetData(), apoapseClient);
		apoapseClient.GetClientUsers().OnAddNewUser(user);
	}
	
	else if (cmd.name == "create_room")
	{
		apoapseClient.GetContentManager().OnAddNewRoom(cmd.GetData());
	}

	else if (cmd.name == "create_thread")
	{
		apoapseClient.GetContentManager().OnAddNewThread(cmd.GetData());
	}

	else if (cmd.name == "new_message")
	{
		apoapseClient.GetContentManager().OnAddNewMessage(cmd.GetData());
	}

	else if (cmd.name == "add_tag")
	{
		apoapseClient.GetContentManager().OnAddNewTag(cmd.GetData());
	}

	LOG_DEBUG << "RECEIVED!";
}
