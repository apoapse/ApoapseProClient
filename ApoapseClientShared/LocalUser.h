#pragma once
#include "CryptographyTypes.hpp"
#include "Username.h"
#include "IUser.h"
class Usergroup;
class JsonHelper;

struct LocalUser : public IUser
{
	Username username;
	std::string nickname;
	const Usergroup* usergroup;

	JsonHelper GetJson() const;

	const Username& GetUsername() const override;
	const Usergroup& GetUsergroup() const override;
//	PublicKeyBytes publicKey;
//	PrivateKeyBytes privateKey;
};