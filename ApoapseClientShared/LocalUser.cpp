#include "stdafx.h"
#include "Common.h"
#include "LocalUser.h"
#include "Json.hpp"
#include "UsergroupManager.h"
#include "User.h"
#include <filesystem>
#include "NativeUI.h"

JsonHelper LocalUser::GetJson() const
{
	const std::string avatarPath = (std::filesystem::exists(NativeUI::GetUserDirectory() + User::GetAvatarFilePath(username)) ? User::GetAvatarFilePath(username) : ""); //TODO OPTIMIZE
	
	JsonHelper ser;
	ser.Insert("nickname", nickname);
	ser.Insert("avatar", avatarPath);
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
