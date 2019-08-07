#include "stdafx.h"
#include "Common.h"
#include "LocalUser.h"
#include "Json.hpp"
#include "UsergroupManager.h"

JsonHelper LocalUser::GetJson() const
{
	JsonHelper ser;
	ser.Insert("nickname", nickname);
	ser.InsertArray<std::string>("permissions", usergroup->GetPermissions());

	return ser;
}
