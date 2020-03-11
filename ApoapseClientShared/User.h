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

#pragma once
#include <vector>
#include <string>
#include "TypeDefs.hpp"
#include "Username.h"
#include <set>
class DataStructure;
class JsonHelper;
class ApoapseClient;
class Usergroup;

class User
{
	ApoapseClient* apoapseClient;
	static constexpr UInt32 usernameHashIterations = 100'000;
	static constexpr UInt32 passwordHashIterationsServer = 100'000;
	//static constexpr UInt32 identityKeySize = 4096;
	//static constexpr UInt32 identityDecryptionKeyHashIterations = 500'000;

public:
	enum class UserStatus
	{
		offine = 0,
		online = 1
	};
	
	User() = default;
	User(DataStructure& data, ApoapseClient& client);

	DbId id = -1;
	Username username;
	const Usergroup* usergroup;
	std::string nickname;
	bool isLocalUser = false;

	JsonHelper GetJson() const;
	UserStatus GetStatus() const;

	static Username HashUsername(const std::string& username);
	static std::vector<byte> HashPasswordForServer(const std::string& password);
	static std::string GenerateTemporaryRandomPassword();
	static std::string GetAvatarFilePath(const Username& username);

	//static std::pair<PrivateKeyBytes, PublicKeyBytes> GenerateIdentityKey();
	//static hashSecBytes HashPasswordForIdentityPrivateKey(const std::string& password);
	//static std::pair<EncryptedPrivateKeyBytes, IV> EncryptIdentityPrivateKey(const PrivateKeyBytes& privateKey, const hashSecBytes& hashedPassword);
	//static PrivateKeyBytes DecryptIdentityPrivateKey(const EncryptedPrivateKeyBytes& encryptedKey, const IV& iv, const hashSecBytes& hashedPassword);

private:
};

class ClientUsers
{
	std::map<Username, User> m_registeredUsers;
	std::set<Username> m_onlineUsers;
	
	ApoapseClient& apoapseClient;

public:
	ClientUsers(ApoapseClient& client);

	void OnAddNewUser(User& user);
	const User& GetUserByUsername(const Username& username) const;
	const User& GetUserById(DbId id) const;
	std::vector<const User*> GetUsers() const;
	UInt64 GetUserCount() const;

	User::UserStatus GetUserStatus(const Username& username) const;
	void ChangeUserStatus(const Username& username, User::UserStatus status);

private:
};