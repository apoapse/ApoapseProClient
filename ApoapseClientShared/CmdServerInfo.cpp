#include "stdafx.h"
#include "Common.h"
#include "CommandsManager.h"
#include "ClientConnection.h"
#include "ApoapseClient.h"
#include "LocalUser.h"
#include "User.h"
#include "MemoryUtils.hpp"

class CmdServerInfo : public Command
{
public:
	CommandInfo& GetInfo() const override
	{
		static auto info = CommandInfo();
		info.command = CommandId::server_info;
		info.clientOnly = true;
		info.onlyNonAuthenticated = true;
		info.fields =
		{
			CommandField{ "status", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, CmdServerInfo::ValidateStatusField) },

			CommandField{ "public_key", FieldRequirement::any_optional, FIELD_VALUE_VALIDATOR(ByteContainer, CommandField::ContainerIsNotEmpty<ByteContainer>) },
			CommandField{ "private_key_encrypted", FieldRequirement::any_optional, FIELD_VALUE_VALIDATOR(ByteContainer, CommandField::ContainerIsNotEmpty<ByteContainer>) },
			CommandField{ "private_key_iv", FieldRequirement::any_optional, FIELD_VALUE_VALIDATOR(ByteContainer, CommandField::ContainerIsNotEmpty<ByteContainer>) },
		};

		return info;
	}
	
private:
	void Process(ClientConnection& sender) override
	{
		auto status = GetFieldsData().GetValue<std::string>("status");

		if (status == "setup_state")
		{
			sender.client.OnSetupState();
		}
		else if (status == "authenticated")
		{
			const IV iv = VectorToArray<byte, 16>(GetFieldsData().GetValue<ByteContainer>("private_key_iv"));

			LocalUser user;
			user.username = sender.client.GetLastLoginTryUsername();
			user.publicKey = GetFieldsData().GetValue<ByteContainer>("public_key");
			user.privateKey = User::DecryptIdentityPrivateKey(GetFieldsData().GetValue<ByteContainer>("private_key_encrypted"), iv, sender.client.GetIdentityPasswordHash());

			sender.client.Authenticate(user);
		}
		else
		{
			LOG << LogSeverity::error << "Received an unhandled status from the server";
		}
	}

	static bool ValidateStatusField(const std::string& str)
	{
		return (str == "authenticated" || str == "setup_state");
	}
};

APOAPSE_COMMAND_REGISTER(CmdServerInfo, CommandId::server_info);