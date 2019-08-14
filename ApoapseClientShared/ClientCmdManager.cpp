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

	else if (cmd.name == "direct_message")
	{
		cmd.GetData().GetField("direct_recipient").SetValue(apoapseClient.GetContentManager().GetCurrentUserPage().relatedUserPtr->username);
		cmd.GetData().GetField("sent_time").SetValue(DateTimeUtils::UTCDateTime::CurrentTime());
		cmd.GetData().GetField("author").SetValue(apoapseClient.GetLocalUser().username);
	}

	return true;
}

bool ClientCmdManager::OnReceivedCommandPre(CommandV2& cmd, GenericConnection& netConnection)
{
	auto& connection = dynamic_cast<ClientConnection&>(netConnection);

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
			LOG << LogSeverity::error << "Rejecting the command " << cmd.name << " because the operation " << operationUuid.GetBytes() << " is already registered";
			return false;
		}
	}

	if (cmd.name == "new_message" || cmd.name == "direct_message")
	{
		// TODO add default value feature to the data system/database integrity system so that is_read is to false by default
		if (cmd.GetData().GetField("author").GetValue<Username>() == connection.GetConnectedUser().value())
		{
			cmd.GetData().GetField("is_read").SetValue(true);
		}
		else
		{
			cmd.GetData().GetField("is_read").SetValue(false);
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
			auto usergroupsDat = cmd.GetData().GetField("usergroups").GetDataArray();
			apoapseClient.InitUsergroupManager(usergroupsDat);

			LocalUser user;
			user.nickname = HTMLUI::HtmlSpecialChars(cmd.GetData().GetField("nickname").GetValue<std::string>(), true);
			user.username = cmd.GetData().GetField("username").GetValue<Username>();
			user.usergroup = &apoapseClient.GetUsergroupManager().GetUsergroup(cmd.GetData().GetField("usergroup").GetValue<Uuid>());

			apoapseClient.Authenticate(user);
		}
		else if (status == "requirePasswordChange")
		{
			LOG << "The server is requesting password change.";
		}
	}

	else if (cmd.name == "user")
	{
		auto user = User(cmd.GetData(), apoapseClient);
		apoapseClient.GetClientUsers().OnAddNewUser(user);
		apoapseClient.GetContentManager().RegisterPrivateMsgThread(apoapseClient.GetClientUsers().GetUserByUsername(user.username));
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

	else if (cmd.name == "direct_message")
	{
		apoapseClient.GetContentManager().OnAddNewPrivateMessage(cmd.GetData());
	}

	else if (cmd.name == "add_tag")
	{
		apoapseClient.GetContentManager().OnAddNewTag(cmd.GetData());
	}

	else if (cmd.name == "mark_as_read")
	{
		if (cmd.GetData().GetField("item_type").GetValue<std::string>() == "msg")
		{
			apoapseClient.GetContentManager().MarkMessageAsRead(cmd.GetData().GetField("item_uuid").GetValue<Uuid>());
		}
		else
		{
			LOG << LogSeverity::warning << "mark_as_read: Unsupported item_type";
		}
	}
}

void ClientCmdManager::OnReceivedCommandPost(CommandV2& cmd, GenericConnection& netConnection)
{

}
