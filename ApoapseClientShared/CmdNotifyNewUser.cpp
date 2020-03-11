// ----------------------------------------------------------------------------
// Copyright (C) 2020 Apoapse
// Copyright (C) 2020 Guillaume Puyal
//
// Distributed under the Apoapse Pro Client Software License. Non-commercial use only.
// See accompanying file LICENSE.md
//
// For more information visit https://github.com/apoapse/
// And https://apoapse.space/
// ----------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "CommandsManager.h"
#include "ClientConnection.h"
#include "ApoapseClient.h"
#include "LocalUser.h"
#include "User.h"
#include "SQLQuery.h"
#include "SQLUtils.hpp"

class CmdNotifyNewUser : public Command
{
public:
	CommandInfo& GetInfo() const override
	{
		static auto info = CommandInfo();
		info.command = CommandId::notify_new_user;
		info.clientOnly = true;
		info.requireAuthentication = true;
		info.fields =
		{
			Field{ "username", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::vector<byte>, Username::IsValid) },
		};

		info.metadataTypes = MetadataAcess::all;
		info.metadataSelfFields =
		{
			Field{ "nickname", FieldRequirement::any_mendatory, FIELD_VALUE_VALIDATOR(std::string, [&](const auto& str) { return !str.empty(); }) },
		};

		return info;
	}

private:
	void Process(ClientConnection& sender) override
	{
		auto metaAll = GetMetadataField(MetadataAcess::all);
		MessagePackDeserializer allData = metaAll.ReadData();

		DbId dbId = SQLUtils::CountRows("users");
		const auto username = Username(GetFieldsData().GetValue<ByteContainer>("username"));

		SQLQuery query(*global->database);
		query << INSERT_INTO << "users" << "(id, username, nickname)" << VALUES << "(" << dbId << "," << GetFieldsData().GetValue<ByteContainer>("username") << "," << allData.GetValue<std::string>("nickname") << ")";
		query.Exec();

		//Operation(OperationType::new_user, username, dbId).Save();
		// TODO ADD to some sort of manager?
	}
};

APOAPSE_COMMAND_REGISTER(CmdNotifyNewUser, CommandId::notify_new_user);