#pragma once
#include "CryptographyTypes.hpp"
#include "Username.h"

struct LocalUser
{
	Username username;
	std::string nickname;
	
//	PublicKeyBytes publicKey;
//	PrivateKeyBytes privateKey;
};