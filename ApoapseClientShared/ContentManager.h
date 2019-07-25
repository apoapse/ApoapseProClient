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

	std::vector<ApoapseThread> threads;

	Room(DataStructure& data);
	bool operator==(const Room& other) const;

	JsonHelper GetJson() const;
};

class ContentManager
{
	ApoapseClient& client;
	std::vector<Room> m_rooms;

	Room* m_selectedRoom = nullptr;

public:
	ContentManager(ApoapseClient& apoapseClient);
	void Init();

	void OnAddNewRoom(DataStructure& data);
	void OnAddNewThread(DataStructure& data);

	void OnReceivedSignal(const std::string& name, const JsonHelper& json);
	
	Room& GetRoomById(DbId id);
	Room& GetRoomByUuid(Uuid uuid);
	void OpenRoom(Room& room);

	Room& GetCurrentRoom();

private:
	void UIRoomsUpdate() const;
};