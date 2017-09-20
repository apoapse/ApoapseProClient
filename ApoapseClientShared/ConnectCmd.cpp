#include "stdafx.h"
#include "Command.h"
#include "Common.h"
#include "CommandsManager.h"

class ConnectCmd : public Command
{
	CommandInfo& GetInfo() const override
	{
		static auto info = CommandInfo();
		info.command = CommandId::connect;
		info.fields =
		{
			CommandField{ "user", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, ConnectCmd::Test) },
		};

		return info;
	}

	void Main()
	{
		std::function<bool(std::string)> test = ConnectCmd::Test;
	}

	static bool Test(const std::string& str)
	{


		return true;
	}

	static bool TestArray(const std::vector<int>& vec)
	{
		for (auto item : vec)
		{
			LOG << item;
		}

		return true;
	}
};

APOAPSE_COMMAND_REGISTER(ConnectCmd, CommandId::connect);