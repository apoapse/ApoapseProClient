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

#include "stdafx.h"
#include "Common.h"
#include "NotificationsManager.h"
#include "ApoapseClient.h"

NotificationsManager::NotificationsManager(ApoapseClient& apoapseClient) : client(apoapseClient)
{
}

void NotificationsManager::NewMessage()
{
	global->htmlUI->SendSignal("icon_blink", "");
}

void NotificationsManager::OnUpdateTotalUnreadMsgCount(Int64 count)
{
	if (count > 0)
		global->htmlUI->SendSignal("set_icon", "ClientResources/active_icon.ico");
	else
		global->htmlUI->SendSignal("set_icon", "ClientResources/default.ico");
}
