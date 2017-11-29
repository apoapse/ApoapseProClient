#include "stdafx.h"
#include "User.h"
#include "Common.h"
#include "Hash.hpp"
#include "RSA.hpp"
#include "AES.hpp"

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
