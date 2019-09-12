#include "stdafx.h"
#include "Common.h"
#include "ClientCmdManager.h"
#include "ClientConnection.h"
#include "CommandsDef.hpp"
#include "LocalUser.h"
#include "ApoapseClient.h"
#include "HTMLUI.h"
#include "ClientFileStreamConnection.h"
#include <filesystem>
#include "FileUtils.h"
#include "ApoapseError.h"

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

	if (cmd.name == "new_message" || cmd.name == "direct_message")
	{
		// Attachments
		const auto attachmentFiles = apoapseClient.GetDroppedFilesToSend();
		if (!attachmentFiles.empty())
		{
			std::vector<DataStructure> attachmentsDat;
			attachmentsDat.reserve(attachmentFiles.size());

			for (const auto& attachment : attachmentFiles)
			{
				attachmentsDat.push_back(attachment.GetDataStructure());
			}
			
			cmd.GetData().GetField("attachments").SetValue(attachmentsDat);
		}

		LOG << "Sending a message with " << attachmentFiles.size() << " attachments";
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
		const Uuid msgUuid = cmd.GetData().GetField("uuid").GetValue<Uuid>();
		
		// TODO add default value feature to the data system/database integrity system so that is_read is to false by default
		if (cmd.GetData().GetField("author").GetValue<Username>() == connection.GetConnectedUser().value())
		{
			cmd.GetData().GetField("is_read").SetValue(true);
		}
		else
		{
			cmd.GetData().GetField("is_read").SetValue(false);
		}

		if (cmd.GetData().GetField("attachments").HasValue())
		{
			auto& attachmentsDat = cmd.GetData().GetField("attachments").GetDataArray();

			for (DataStructure& dat : attachmentsDat)
			{
				dat.GetField("parent_message").SetValue(msgUuid);
				dat.GetField("is_downloaded").SetValue(false);
				dat.SaveToDatabase();
			}
		}
	}

	return true;
}

void ClientCmdManager::OnReceivedCommand(CommandV2& cmd, GenericConnection& netConnection)
{
	auto& connection = dynamic_cast<ClientConnection&>(netConnection);

	if (cmd.name == "error")
	{
		connection.client.OnReceivedError(ApoapseError(cmd));
	}

	else if (cmd.name == "server_info")
	{
		const auto status = cmd.GetData().GetField("status").GetValue<std::string>();

		if (status == "authenticated")
		{
			// Read usergroups
			{
				auto& usergroupsDat = cmd.GetData().GetField("usergroups").GetDataArray();
				apoapseClient.InitUsergroupManager(usergroupsDat);
			}

			// Read local user
			LocalUser user;
			user.nickname = HTMLUI::HtmlSpecialChars(cmd.GetData().GetField("nickname").GetValue<std::string>(), true);
			user.username = cmd.GetData().GetField("username").GetValue<Username>();
			user.usergroup = &apoapseClient.GetUsergroupManager().GetUsergroup(cmd.GetData().GetField("usergroup").GetValue<Uuid>());

			// Auth
			apoapseClient.Authenticate(user);
			apoapseClient.AuthenticateFileStream(cmd.GetData().GetField("file_stream_auth_code").GetValue<ByteContainer>());

			// Read online users
			{
				auto& onlineUsers = cmd.GetData().GetField("connected_users").GetDataArray();
				for (DataStructure& dat : onlineUsers)
				{
					apoapseClient.GetClientUsers().ChangeUserStatus(dat.GetField("user").GetValue<Username>(), User::UserStatus::online);
				}

				LOG << onlineUsers.size() << " users are online";
				
				if (!onlineUsers.empty())
					apoapseClient.GetContentManager().UIUserListUpdate();
			}
		}
		else if (status == "requirePasswordChange")
		{
			LOG << "The server is requesting password change.";
		}
	}

	else if (cmd.name == "user")
	{
		if (cmd.GetData().GetField("avatar").HasValue())
		{
			const auto username = cmd.GetData().GetField("username").GetValue<Username>();
			const std::string filePath = User::GetAvatarFilePath(username);

			if (!std::filesystem::exists(filePath))
				FileUtils::SaveBytesToFile(filePath, cmd.GetData().GetField("avatar").GetValue<ByteContainer>());
		}
		
		auto user = User(cmd.GetData(), apoapseClient);
		apoapseClient.GetClientUsers().OnAddNewUser(user);
		apoapseClient.GetContentManager().RegisterPrivateMsgThread(apoapseClient.GetClientUsers().GetUserByUsername(user.username));
	}

	else if (cmd.name == "change_user_status")
	{
		const Username username = cmd.GetData().GetField("user").GetValue<Username>();
		const User::UserStatus status = static_cast<User::UserStatus>(cmd.GetData().GetField("status").GetValue<Int64>());
		
		apoapseClient.GetClientUsers().ChangeUserStatus(username, status);
		apoapseClient.GetContentManager().UIUserListUpdate();
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

	else if (cmd.name == "ready_to_receive_file")
	{
		apoapseClient.SendQueuedDroppedFile();
	}
}

void ClientCmdManager::OnReceivedCommandPost(CommandV2& cmd, GenericConnection& netConnection)
{

}
