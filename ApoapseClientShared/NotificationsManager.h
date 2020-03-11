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

class NotificationsManager
{
	class ApoapseClient& client;
	
public:
	NotificationsManager(ApoapseClient& apoapseClient);
	
	void NewMessage();
	void OnUpdateTotalUnreadMsgCount(Int64 count);
	
private:
};