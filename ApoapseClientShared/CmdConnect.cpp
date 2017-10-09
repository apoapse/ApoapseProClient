#include "stdafx.h"
#include "CmdConnect.h"
#include "Common.h"
#include "CommandsManager.h"
#include "Hash.hpp"
#include "ProtocolVersion.hpp"

CommandInfo& CmdConnect::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::connect;
	info.serverOnly = true;

	return info;
}

void CmdConnect::PrepareLoginCmd(const std::string& username, const std::string& password)
{
	m_serializedData = MessagePackSerializer();

	m_serializedData->Group("",
	{
		MSGPK_ORDERED_APPEND(m_serializedData.value(), "protocol_version", protocolVersion),
		MSGPK_ORDERED_APPEND(m_serializedData.value(), "username",  HashUsername(username)),
		MSGPK_ORDERED_APPEND(m_serializedData.value(), "password",  HashPassword(password)),
	});
}

std::vector<byte> CmdConnect::HashUsername(const std::string& username)
{
	std::vector<byte> output;
	auto digest = Cryptography::SHA3_256(std::vector<byte>(username.begin(), username.end()));
	output.insert(output.begin(), digest.begin(), digest.end());

	return output;
}

std::vector<byte> CmdConnect::HashPassword(const std::string& password)
{
	std::vector<byte> output;
	auto digest = Cryptography::PBKDF2_SHA256(std::vector<byte>(password.begin(), password.end()), std::vector<byte>(), passwordHashIterations);
	output.insert(output.begin(), digest.begin(), digest.end());

	return output;
}

APOAPSE_COMMAND_REGISTER(CmdConnect, CommandId::connect);