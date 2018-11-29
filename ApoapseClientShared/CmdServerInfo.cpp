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
			Field{ "status", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, CmdServerInfo::ValidateStatusField) },
			Field{ "requirePasswordChange", FieldRequirement::any_optional, FIELD_VALUE(bool) },
			Field{ "username", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Username::IsValid) },

// 			Field{ "public_key", FieldRequirement::any_optional, FIELD_VALUE_VALIDATOR(ByteContainer, Field::ContainerIsNotEmpty<ByteContainer>) },
// 			Field{ "private_key_encrypted", FieldRequirement::any_optional, FIELD_VALUE_VALIDATOR(ByteContainer, Field::ContainerIsNotEmpty<ByteContainer>) },
// 			Field{ "private_key_iv", FieldRequirement::any_optional, FIELD_VALUE_VALIDATOR(ByteContainer, Field::ContainerIsNotEmpty<ByteContainer>) },
		};

		info.metadataTypes = MetadataAcess::all;
		info.metadataSelfFields =
		{
			Field{ "nickname", FieldRequirement::any_optional, FIELD_VALUE_VALIDATOR(std::string, [&](const auto& str) { return !str.empty(); }) },
		};

		return info;
	}
	
private:
	void Process(ClientConnection& sender) override
	{
		const auto status = GetFieldsData().GetValue<std::string>("status");
		const auto requirePasswordChange = GetFieldsData().GetValueOptional<bool>("requirePasswordChange");

		if (status == "setup_state")
		{
			sender.client.OnSetupState();
		}
		else if (status == "authenticated")
		{
			if (requirePasswordChange.has_value() && requirePasswordChange.value() == true)
			{
				sender.client.OnUserFirstConnection();
			}
			else
			{
				//const IV iv = VectorToArray<byte, 16>(GetFieldsData().GetValue<ByteContainer>("private_key_iv")); #MVP

				//user.publicKey = GetFieldsData().GetValue<ByteContainer>("public_key");
				//user.privateKey = User::DecryptIdentityPrivateKey(GetFieldsData().GetValue<ByteContainer>("private_key_encrypted"), iv, sender.client.GetIdentityPasswordHash());

				auto metaAll = GetMetadataField(MetadataAcess::all);
				auto metadataAll = metaAll.ReadData();


				User user;
				user.nickname = metadataAll.GetValue<std::string>("nickname");
				user.username = Username(GetFieldsData().GetValue<ByteContainer>("username"));

				sender.client.Authenticate(user);
			}
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