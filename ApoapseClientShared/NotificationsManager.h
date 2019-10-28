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