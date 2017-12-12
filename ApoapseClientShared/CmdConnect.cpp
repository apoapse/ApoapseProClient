#include "stdafx.h"
#include "CmdConnect.h"
#include "Common.h"
#include "CommandsManager.h"
#include "User.h"
#include "ProtocolVersion.hpp"

CommandInfo& CmdConnect::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::connect;
	info.serverOnly = true;
	info.onlyNonAuthenticated = true;

	return info;
}

void CmdConnect::PrepareLoginCmd(const Username& username, const std::string& password)
{
	m_serializedData = MessagePackSerializer();

	m_serializedData->Group("",
	{
		MSGPK_ORDERED_APPEND(m_serializedData.value(), "protocol_version", protocolVersion),
		MSGPK_ORDERED_APPEND(m_serializedData.value(), "username", username.GetRaw()),
		MSGPK_ORDERED_APPEND(m_serializedData.value(), "password", User::HashPasswordForServer(password)),
	});
}

//APOAPSE_COMMAND_REGISTER(CmdConnect, CommandId::connect);