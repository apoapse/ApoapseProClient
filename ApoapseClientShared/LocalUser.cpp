#include "stdafx.h"
#include "Common.h"
#include "LocalUser.h"
#include "Json.hpp"
#include "UsergroupManager.h"
#include "User.h"

JsonHelper LocalUser::GetJson() const
{
	JsonHelper ser;
	ser.Insert("nickname", nickname);
	ser.Insert("avatar", User::GetAvatarFilePath(username));
	ser.InsertArray<std::string>("permissions", usergroup->GetPermissions());

	return ser;
}

const Username& LocalUser::GetUsername() const
{
	return username;
}

const Usergroup& LocalUser::GetUsergroup() const
{
	return *usergroup;
}
