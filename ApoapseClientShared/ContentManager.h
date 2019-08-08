#pragma once
#include "TypeDefs.hpp"
#include "Uuid.h"
#include "ApoapseThread.h"
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

	std::vector<std::unique_ptr<ApoapseThread>> threads;

	Room(DataStructure& data);
	bool operator==(const Room& other) const;

	JsonHelper GetJson() const;
	ApoapseThread& GetThread(DbId id);
};

class ContentManager
{
	std::vector<std::unique_ptr<Room>> m_rooms;

	Room* m_selectedRoom = nullptr;
	ApoapseThread* m_selectedThread = nullptr;

public:
	ApoapseClient& client;

	ContentManager(ApoapseClient& apoapseClient);
	void Init();

	void OnAddNewRoom(DataStructure& data);
	void OnAddNewThread(DataStructure& data);
	void OnAddNewMessage(DataStructure& data);
	void OnAddNewTag(DataStructure& data);

	void OnReceivedSignal(const std::string& name, const JsonHelper& json);
	
	Room& GetRoomById(DbId id);
	Room& GetRoomByUuid(const Uuid& uuid);
	ApoapseThread& GetThreadByUuid(const Uuid& uuid);
	void OpenRoom(Room& room);
	void OpenThread(ApoapseThread& thread);

	Room& GetCurrentRoom();
	bool IsThreadDisplayed() const;
	ApoapseThread& GetCurrentThread();

private:
	void UIRoomsUpdate() const;
};