#include "stdafx.h"
#include "RoomManager.h"
#include "Common.h"
#include "CmdCreateRoom.h"
#include "Operation.h"
#include "ApoapseClient.h"
#include "Json.hpp"
#include "ByteUtils.hpp" // TEMP
#include "HTMLUI.h"

RoomManager::RoomManager(ApoapseClient& client) : apoapseClient(client)
{

}

void RoomManager::Initialize()
{
	UpdateUI();
}

void RoomManager::SendCreateNewRoom(const std::string& name)
{
	ApoapseRoom room;
	room.uuid = Uuid::Generate();

	CmdCreateRoom::SendCreateRoom(room, apoapseClient);
}

void RoomManager::AddNewRoomFromServer(const ApoapseRoom& room)
{
	m_rooms.push_back(room);

	// SAVE

	UpdateUI();
}

void RoomManager::UpdateUI()
{
	JsonHelper ser;

	int i = 0;
	for (const auto& room : m_rooms)
	{
		JsonHelper serRoom;
		serRoom.Insert("internal_id", i);
		serRoom.Insert("name", BytesToHexString(room.uuid.GetAsByteVector())); // TEMP

		ser.Insert("rooms", serRoom);
		i++;
	}
	
	global->htmlUI->SendSignal("rooms_update", ser.Generate());
}
