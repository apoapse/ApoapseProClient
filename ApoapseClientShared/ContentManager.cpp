#include "stdafx.h"
#include "Common.h"
#include "ContentManager.h"
#include "Json.hpp"

Room::Room(DataStructure& data)
{
	name = data.GetField("name").GetValue<std::string>();
	uuid = data.GetField("uuid").GetValue<Uuid>();
	threadsLayout = (data.GetField("threads_layout").GetValue<std::string>() == "single") ? ThreadsLayout::single : ThreadsLayout::multiple;
	id = data.GetDbId();
}

bool Room::operator==(const Room& other) const
{
	return (id == other.id);
}

JsonHelper Room::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("name", name);

	return ser;
}

ContentManager::ContentManager(ApoapseClient& apoapseClient) : client(apoapseClient)
{
	//global content cache system (nothing todo with signaks)
}

void ContentManager::Init()
{
	// Loading rooms
	auto rooms = global->apoapseData->ReadListFromDatabase("room", "", "");
	for (auto& roomData : rooms)
	{
		m_rooms.push_back(Room(roomData));
	}

	if (!m_rooms.empty())
	{
		OpenRoom(m_rooms[0]);
	}

	UIRoomsUpdate();
}

void ContentManager::OnAddNewRoom(DataStructure& data)
{
	auto room = Room(data);
	m_rooms.push_back(room);

	LOG << "Added new room " << room.name;
	UIRoomsUpdate();
}

void ContentManager::OnReceivedSignal(const std::string& name, const JsonHelper& json)
{
	if (name == "loadRoomUI")
	{
		Room& room = GetRoomById(json.ReadFieldValue<Int64>("id").value());
		OpenRoom(room);
	}
}

Room& ContentManager::GetRoomById(DbId id)
{
	auto res = std::find_if(m_rooms.begin(), m_rooms.end(), [id](const Room& room)
	{
		return (room.id == id);
	});

	if (res == m_rooms.end())
		throw std::exception("Unable to find the room with the provided id");

	return *res;
}

void ContentManager::OpenRoom(Room& room)
{
	m_selectedRoom = &room;

	LOG << "Selected room " << room.name;
	UIRoomsUpdate();

	global->htmlUI->SendSignal("OnOpenRoom", "{}");
}

void ContentManager::UIRoomsUpdate() const
{
	JsonHelper ser;

	for (const auto& room : m_rooms)
	{
		auto json = room.GetJson();

		const bool isSelected = (m_selectedRoom && room == *m_selectedRoom);
		json.Insert("is_selected", isSelected);

		ser.Insert("rooms", json);
	}

	global->htmlUI->SendSignal("rooms_update", ser.Generate());
}
