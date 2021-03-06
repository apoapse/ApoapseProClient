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
#include "User.h"
#include "Common.h"
#include "Hash.hpp"
#include "Random.hpp"
#include "Maths.hpp"
#include "Json.hpp"
#include "HTMLUI.h"
#include "ApoapseClient.h"
#include <filesystem>
#include "NativeUI.h"

User::User(DataStructure& data, ApoapseClient& client) : apoapseClient(&client)
{
	username = data.GetField("username").GetValue<Username>();
	nickname = HTMLUI::HtmlSpecialChars(data.GetField("nickname").GetValue<std::string>());
	id = data.GetDbId();

	if (username == apoapseClient->GetLocalUser().username)
	{
		isLocalUser = true;
	}

	usergroup = &apoapseClient->GetUsergroupManager().GetUsergroup(data.GetField("usergroup").GetValue<Uuid>());
}

JsonHelper User::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("nickname", nickname);
	ser.Insert("isOnline", (GetStatus() == UserStatus::online));

	return ser;
}

User::UserStatus User::GetStatus() const
{
	return apoapseClient->GetClientUsers().GetUserStatus(username);
}

Username User::HashUsername(const std::string& username)
{
	std::vector<byte> output;
	auto digest = Cryptography::PBKDF2_SHA256(std::vector<byte>(username.begin(), username.end()), std::vector<byte>(), usernameHashIterations);
	output.insert(output.begin(), digest.begin(), digest.end());

	return Username(output);
}

std::vector<byte> User::HashPasswordForServer(const std::string& password)
{
	std::vector<byte> output;
	auto digest = Cryptography::PBKDF2_SHA256(std::vector<byte>(password.begin(), password.end()), std::vector<byte>(), passwordHashIterationsServer);
	output.insert(output.begin(), digest.begin(), digest.end());

	return output;
}

std::string User::GenerateTemporaryRandomPassword()
{
	constexpr int maxLength = 25;
	constexpr int minLength = 8;

	return Cryptography::GenerateRandomCharacters(minLength, maxLength);
}

std::string User::GetAvatarFilePath(const Username& username)
{
	const std::string path = "client_avatar/av_" + username.ToStr().substr(0, 24) + ".jpg";
	
	return path;
}

/*std::pair<PrivateKeyBytes, PublicKeyBytes> User::GenerateIdentityKey()
{
	return Cryptography::RSA::GenerateKeyPair(identityKeySize);
}

hashSecBytes User::HashPasswordForIdentityPrivateKey(const std::string& password)
{
	const auto firstPass = Cryptography::PBKDF2_SHA256(std::vector<byte>(password.begin(), password.end()), std::vector<byte>(), identityDecryptionKeyHashIterations);
	const auto secondPass = Cryptography::SHA3_384(std::vector<byte>(firstPass.begin(), firstPass.end()));

	return hashSecBytes(secondPass.data(), sha256Length);
}

std::pair<EncryptedPrivateKeyBytes, IV> User::EncryptIdentityPrivateKey(const PrivateKeyBytes& privateKey, const hashSecBytes& hashedPassword)
{
	auto msg = std::vector<byte>(privateKey.begin(), privateKey.end());
	const auto[encryptedPrivateKey, iv] = Cryptography::AES_CBC::EncryptWithKey(msg, hashedPassword);
	msg.clear();

	return std::make_pair(encryptedPrivateKey, iv);
}

PrivateKeyBytes User::DecryptIdentityPrivateKey(const EncryptedPrivateKeyBytes& encryptedKey, const IV& iv, const hashSecBytes& hashedPassword)
{
	const auto res = Cryptography::AES_CBC::Decrypt(hashedPassword, iv, encryptedKey);

	return PrivateKeyBytes(res.data(), res.size());
}
*/

ClientUsers::ClientUsers(ApoapseClient& client): apoapseClient(client)
{
	auto usersData = global->apoapseData->ReadListFromDatabase("user", "", "");

	for (auto& userDat : usersData)
	{
		auto user = User(userDat, apoapseClient);
		m_registeredUsers[user.username] = user;
	}

	LOG << "Loaded " << m_registeredUsers.size() << " users";
}

void ClientUsers::OnAddNewUser(User& user)
{
	m_registeredUsers[user.username] = user;
}

const User& ClientUsers::GetUserByUsername(const Username& username) const
{
	return m_registeredUsers.at(username);
}

const User& ClientUsers::GetUserById(DbId id) const
{
	for (const auto& user : m_registeredUsers)
	{
		if (user.second.id == id)
			return user.second;
	}

	throw std::exception("Unable to find an user with the provided id");
}

std::vector<const User*> ClientUsers::GetUsers() const
{
	std::vector<const User*> ouput;
	ouput.reserve(m_registeredUsers.size());

	for (const auto& user : m_registeredUsers)
	{
		ouput.push_back(&user.second);
	}

	std::sort(ouput.begin(), ouput.end(), [](const User* left, const User* right)
	{
		return (left->isLocalUser > right->isLocalUser);
	});
	
	return ouput;
}

UInt64 ClientUsers::GetUserCount() const
{
	return m_registeredUsers.size();
}

User::UserStatus ClientUsers::GetUserStatus(const Username& username) const
{
	if (m_onlineUsers.count(username))
		return User::UserStatus::online;
	else
		return User::UserStatus::offine;
}

void ClientUsers::ChangeUserStatus(const Username& username, User::UserStatus status)
{
	const bool isOnOnlineList = m_onlineUsers.count(username);

	if (status == User::UserStatus::online && !isOnOnlineList)
	{
		m_onlineUsers.insert(username);
	}
	else if (status == User::UserStatus::offine && isOnOnlineList)
	{
		m_onlineUsers.erase(username);
	}
}
