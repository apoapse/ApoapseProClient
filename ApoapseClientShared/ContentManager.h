#pragma once
#include "TypeDefs.hpp"
#include "Uuid.h"
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
	void OnReceivedSignal(const std::string& name, const JsonHelper& json);
	
	Room& GetRoomById(DbId id);
	void OpenRoom(Room& room);

private:
	void UIRoomsUpdate() const;

};