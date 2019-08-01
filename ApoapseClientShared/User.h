#pragma once
#include <vector>
#include <string>
#include "TypeDefs.hpp"
#include "Username.h"
#include <map>
class DataStructure;
class JsonHelper;
class ApoapseClient;

class User
{
	ApoapseClient* apoapseClient;
	static constexpr UInt32 usernameHashIterations = 100'000;
	static constexpr UInt32 passwordHashIterationsServer = 100'000;
	//static constexpr UInt32 identityKeySize = 4096;
	//static constexpr UInt32 identityDecryptionKeyHashIterations = 500'000;

public:
	User() = default;
	User(DataStructure& data, ApoapseClient& client);

	DbId id = -1;
	Username username;
	std::string nickname;
	bool isOnline = false;
	bool isLocalUser = false;

	JsonHelper GetJson() const;

	static Username HashUsername(const std::string& username);
	static std::vector<byte> HashPasswordForServer(const std::string& password);
	static std::string GenerateTemporaryRandomPassword();

	//static std::pair<PrivateKeyBytes, PublicKeyBytes> GenerateIdentityKey();
	//static hashSecBytes HashPasswordForIdentityPrivateKey(const std::string& password);
	//static std::pair<EncryptedPrivateKeyBytes, IV> EncryptIdentityPrivateKey(const PrivateKeyBytes& privateKey, const hashSecBytes& hashedPassword);
	//static PrivateKeyBytes DecryptIdentityPrivateKey(const EncryptedPrivateKeyBytes& encryptedKey, const IV& iv, const hashSecBytes& hashedPassword);

private:
};

class ClientUsers
{
	std::map<Username, User> m_registeredUsers;
	ApoapseClient& apoapseClient;

public:
	ClientUsers(ApoapseClient& client);

	void OnAddNewUser(User& user);
	const User& GetUserByUsername(const Username& username) const;

private:
	void UpdateUI() const;
};