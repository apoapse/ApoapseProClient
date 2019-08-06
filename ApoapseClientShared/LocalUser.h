#pragma once
#include "CryptographyTypes.hpp"
#include "Username.h"
class Usergroup;

struct LocalUser
{
	Username username;
	std::string nickname;
	const Usergroup* usergroup;
	
//	PublicKeyBytes publicKey;
//	PrivateKeyBytes privateKey;
};