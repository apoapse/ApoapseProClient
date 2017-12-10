#pragma once
#include "Command.h"
#include "IUsersManager.h"
#include "Username.h"
#include "CryptographyTypes.hpp"
class ApoapseClient;

class DummyUsersManager : public IUsersManager
{
	const Username& m_uniqueUser;
	const PublicKeyBytes& m_userIdKey;

public:
	DummyUsersManager(const Username& uniqueUser, const PublicKeyBytes& idPublicKey)
		: m_uniqueUser(uniqueUser)
		, m_userIdKey(idPublicKey)
	{
	}

	bool DoesUserExist(const Username& username) const override
	{
		return (username == m_uniqueUser);
	}

	PublicKeyBytes GetUserIdentityPublicKey(const Username& username) const override
	{
		return m_userIdKey;
	}
};

class CmdCreateInitialAdmin : public Command
{

public:
	CommandInfo& GetInfo() const override;

	static void CreateAndSend(const std::string& username, const std::string& password, ApoapseClient& client);
};