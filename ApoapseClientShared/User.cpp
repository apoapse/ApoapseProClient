#include "stdafx.h"
#include "User.h"
#include "Common.h"
#include "Hash.hpp"
#include "RSA.hpp"
#include "AES.hpp"
#include "Random.hpp"
#include "Maths.hpp"
#include "SQLQuery.h"

User User::GetUserByUsername(const Username& username)
{
	User user;

	SQLQuery query(*global->database);
	query << SELECT << "nickname" << FROM << "users" << WHERE "username" << EQUALS << username.GetRaw();
	auto res = query.Exec();

	user.username = username;
	user.nickname = res[0][0].GetText();

	return user;
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
	std::string output;
	constexpr int maxLength = 25;
	constexpr int minLength = 8;

	auto bytes = Cryptography::GenerateRandomBytes(35);

	for (const auto& cByte : bytes)
	{
		if (output.size() >= maxLength)
			return output;

		// We make sure to use only certain ascii chars/bytes from the original random bytes string
		if (cByte != 0x22 && cByte != 0x27 && (IsInBound<byte>(cByte, 0x21, 0x3B) || IsInBound<byte>(cByte, 0x41, 0x5A) || IsInBound<byte>(cByte, 0x61, 0x7A)))
			output += cByte;
	}

	if (output.size() < minLength)
		return GenerateTemporaryRandomPassword();
	else
		return output;
}

std::pair<PrivateKeyBytes, PublicKeyBytes> User::GenerateIdentityKey()
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
