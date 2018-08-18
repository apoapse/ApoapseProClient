#pragma once
#include <vector>
#include <string>
#include "TypeDefs.hpp"
#include "Username.h"

class User
{
	static constexpr UInt32 usernameHashIterations = 100'000;
	static constexpr UInt32 passwordHashIterationsServer = 100'000;
	static constexpr UInt32 identityKeySize = 4096;
	static constexpr UInt32 identityDecryptionKeyHashIterations = 500'000;

public:
	static Username HashUsername(const std::string& username);
	static std::vector<byte> HashPasswordForServer(const std::string& password);
	static std::string GenerateTemporaryRandomPassword();

	static std::pair<PrivateKeyBytes, PublicKeyBytes> GenerateIdentityKey();
	static hashSecBytes HashPasswordForIdentityPrivateKey(const std::string& password);
	static std::pair<EncryptedPrivateKeyBytes, IV> EncryptIdentityPrivateKey(const PrivateKeyBytes& privateKey, const hashSecBytes& hashedPassword);
	static PrivateKeyBytes DecryptIdentityPrivateKey(const EncryptedPrivateKeyBytes& encryptedKey, const IV& iv, const hashSecBytes& hashedPassword);

private:
};