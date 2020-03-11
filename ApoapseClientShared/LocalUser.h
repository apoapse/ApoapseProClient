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