#include "stdafx.h"
#include "Common.h"
#include "CommandsManager.h"
#include "ClientConnection.h"
#include "ApoapseClient.h"

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
			CommandField{ "status", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, CmdServerInfo::ValidateStatusField) }
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
	}

	static bool ValidateStatusField(const std::string& str)
	{
		return (str == "authenticated" || str == "setup_state");
	}
};

APOAPSE_COMMAND_REGISTER(CmdServerInfo, CommandId::server_info);