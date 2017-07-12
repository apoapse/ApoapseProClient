#include "stdafx.h"
#include "Command.h"
#include "Common.h"
#include "CommandsManager.h"

class ConnectCmd : public Command
{
	CommandInfo& GetInfo() const override
	{
		static auto info = CommandInfo();
		info.command = Commands::CONNECT;
		info.fields =
		{
			CommandField{ "user", FieldRequirement::ANY_MENDATORY, FIELD_VALUE_VALIDATOR(std::string, ConnectCmd::Test) },
			CommandField{ "pass", FieldRequirement::ANY_MENDATORY, FIELD_VALUE(std::string) },
			CommandField{ "test_array", FieldRequirement::ANY_OPTIONAL, FIELD_ARRAY_VALIDATOR(int, ConnectCmd::TestArray) }
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

APOAPSE_COMMAND_REGISTER(ConnectCmd, Commands::CONNECT);