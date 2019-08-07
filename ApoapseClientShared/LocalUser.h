#pragma once
#include "CryptographyTypes.hpp"
#include "Username.h"
class Usergroup;
class JsonHelper;

struct LocalUser
{
	Username username;
	std::string nickname;
	const Usergroup* usergroup;

	JsonHelper GetJson() const;
	
//	PublicKeyBytes publicKey;
//	PrivateKeyBytes privateKey;
};