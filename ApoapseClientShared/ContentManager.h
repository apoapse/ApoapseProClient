#pragma once
#include "TypeDefs.hpp"
#include "Uuid.h"
#include "ApoapseThread.h"
#include "PrivateMsgThread.h"
class ApoapseClient;

struct Room
{
	enum class ThreadsLayout
	{
		multiple,
		single
	};

	std::string name;
	Uuid uuid;
	ThreadsLayout threadsLayout = ThreadsLayout::multiple;
	DbId id = -1;
	Int64 unreadMsgCount = 0;

	std::vector<std::unique_ptr<ApoapseThread>> threads;

	Room(DataStructure& data);
	void RefrechUnreadMessagesCount();
	bool operator==(const Room& other) const;

	JsonHelper GetJson() const;
	ApoapseThread& GetThread(DbId id);
};

class ContentManager
{
	std::vector<std::unique_ptr<Room>> m_rooms;
	std::vector<std::unique_ptr<PrivateMsgThread>> m_privateMsgThreads;

	Room* m_selectedRoom = nullptr;
	ApoapseThread* m_selectedThread = nullptr;
	PrivateMsgThread* m_selectedUserPage = nullptr;

public:
	ApoapseClient& client;

	ContentManager(ApoapseClient& apoapseClient);
	void Init();

	void OnAddNewRoom(DataStructure& data);
	void OnAddNewThread(DataStructure& data);
	void OnAddNewMessage(DataStructure& data);
	void OnAddNewPrivateMessage(DataStructure& data);
	void OnAddNewTag(DataStructure& data);
	void MarkMessageAsRead(const Uuid& uuid);

	void OnReceivedSignal(const std::string& name, const JsonHelper& json);
	
	Room& GetRoomById(DbId id);
	Room& GetRoomByUuid(const Uuid& uuid);
	ApoapseThread& GetThreadByUuid(const Uuid& uuid);
	ApoapseThread& GetThreadById(DbId id);
	PrivateMsgThread& GetPrivateThreadByUserId(DbId id);
	
	void OpenRoom(Room& room);
	void OpenThread(ApoapseThread& thread);
	void OpenPrivateMsgThread(PrivateMsgThread& thread);

	void RegisterPrivateMsgThread(const User& user);

	Room& GetCurrentRoom() const;
	bool IsThreadDisplayed() const;
	ApoapseThread& GetCurrentThread() const;
	bool IsUserPageDisplayed() const;
	PrivateMsgThread& GetCurrentUserPage() const;

	void UIRoomsUpdate() const;

private:

};