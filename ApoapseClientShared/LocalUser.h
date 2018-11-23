#pragma once
#include "CryptographyTypes.hpp"
#include "Username.h"
#include "User.h"

struct LocalUser : public User
{
	LocalUser(User& user)
	{
		username = user.username;
		nickname = user.nickname;
	}
	
//	PublicKeyBytes publicKey;
//	PrivateKeyBytes privateKey;
};