#include "stdafx.h"
#include "RoomManager.h"
#include "Common.h"
#include "CmdCreateRoom.h"
#include "Operation.h"
#include "ApoapseClient.h"
#include "Json.hpp"
#include "ByteUtils.hpp" // TEMP
#include "HTMLUI.h"
#include "SQLQuery.h"
#include "CmdCreateThread.h"
#include "SQLUtils.hpp"

RoomManager::RoomManager(ApoapseClient& client) : apoapseClient(client)
{

}

void RoomManager::Initialize()
{
	{
		SQLQuery query(*global->database);
		query << SELECT << ALL << FROM << "rooms" << ORDER_BY << "id" << ASC;
		auto res = query.Exec();

		for (const auto& row : res)
		{
			auto room = std::make_unique<ApoapseRoom>();
			room->dbId = row[0].GetInt64();
			room->uuid = Uuid(row[1].GetByteArray());
			room->name = row[2].GetText();

			m_rooms.push_back(std::move(room));
		}
	}

	if (!m_rooms.empty())
		SetUISelectedRoom(0);	// #TODO #MVP Select the last room used by the user, not the first one in the list like currently

	UpdateUI();

	LoadThreadsLists();
	UpdateThreadListUI();
}

void RoomManager::SendCreateNewRoom(const std::string& name)
{
	ApoapseRoom room;
	room.uuid = Uuid::Generate();
	room.name = name;

	CmdCreateRoom::SendCreateRoom(room, apoapseClient);
}

void RoomManager::AddNewRoomFromServer(std::unique_ptr<ApoapseRoom> room)
{
	if (GetRoomByUuid(room->uuid) != nullptr)
	{
		LOG << LogSeverity::error << "Trying to add a room that is already here";
		return;
	}

	// SAVE
	{
		DbId newId = m_rooms.size();

		SQLQuery query(*global->database);
		query << INSERT_INTO << "rooms" << " (id, uuid, name)" << VALUES << "(" << newId << "," << room->uuid.GetInRawFormat() << "," << room->name << ")";
		query.Exec();

		Operation(OperationType::new_room, apoapseClient.GetLocalUser().username, newId).Save();
	}

	m_rooms.push_back(std::move(room));

	if (m_rooms.size() == 1)
		SetUISelectedRoom(0);	// If the user created his first room, we automatically switch to it

	UpdateUI();
}

void RoomManager::SetUISelectedRoom(UInt64 internalRoomId)
{
	if (internalRoomId <= (m_rooms.size() - 1))
	{
		m_uiSelectedRoom = m_rooms[internalRoomId].get();
		LOG << "Selected room " << m_uiSelectedRoom->uuid.GetAsByteVector();

		UpdateThreadListUI();
	}
	else
	{
		LOG << LogSeverity::error << "Invalid internalRoomId";
	}
}

const ApoapseRoom* RoomManager::GetSelectedRoom() const
{
	return m_uiSelectedRoom;
}

ApoapseRoom* RoomManager::GetRoomByUuid(const Uuid& uuid) const
{
	const auto res = std::find_if(m_rooms.begin(), m_rooms.end(), [&](const auto& room)
	{
		return (room->uuid == uuid);
	});

	return ((res != m_rooms.end()) ? res->get() : nullptr);
}

void RoomManager::SetActiveThread(Int64 id)
{
	ASSERT(m_uiSelectedRoom != nullptr);
	const auto& thread = GetSelectedRoom()->threads.at(id);

	m_selectedThread = ApoapseThread(thread, this); // #TODO cache the Apoapse threads?
	//LOG << "Apoapse thread " << thread.uuid.GetAsByteVector() << " set to active";

	m_selectedThread->LoadMessages();
	m_selectedThread->OnUIDisplay();
}

void RoomManager::SendAddNewThread(const std::string& name)
{
	ASSERT(GetSelectedRoom() != nullptr);
	const auto uuid = Uuid::Generate();

	CmdCreateThread::SendCreateThread(uuid, GetSelectedRoom()->uuid, name, apoapseClient);
}

void RoomManager::AddNewThreadFromServer(const Uuid& uuid, const Uuid& roomUuid, const std::string& name)
{
	ASSERT(m_uiSelectedRoom != nullptr);
	const auto relatedRoomUuid = GetRoomByUuid(roomUuid);

	if (relatedRoomUuid == nullptr)
	{
		LOG << LogSeverity::error << "Trying to add the thread " << uuid.GetAsByteVector() << " to a room that does not exist";
		return;
	}

	if (GetThreadByUuid(uuid) != nullptr)
	{
		LOG << LogSeverity::error << "Trying to add a new thread that is already here";
		return;
	}

	SimpleApoapseThread thread;
	thread.dbId = SQLUtils::CountRows("threads");
	thread.uuid = uuid;
	thread.roomUuid = roomUuid;
	thread.name = name;

	// SAVE
	{
		SQLQuery query(*global->database);
		query << INSERT_INTO << "threads" << " (id, uuid, room_uuid, name)" << VALUES << "(" << thread.dbId << "," << thread.uuid.GetInRawFormat() << "," << thread.roomUuid.GetInRawFormat() << "," << thread.name << ")";
		query.Exec();

		Operation(OperationType::new_thread, apoapseClient.GetLocalUser().username, thread.dbId).Save();
	}

	relatedRoomUuid->threads.push_back(thread);

	if (relatedRoomUuid == GetSelectedRoom())
	{
		const UIId uiId = (relatedRoomUuid->threads.size() - 1);
		OnNewThreadAddedToCurrentRoom(thread, uiId);
	}
}

ApoapseThread* RoomManager::GetActiveThread()
{
	return (m_selectedThread.has_value() ? &m_selectedThread.value() : nullptr);
}

SimpleApoapseThread* RoomManager::GetThreadByUuid(const Uuid& uuid)
{
	for (const auto& room : m_rooms)
	{
		const auto res = std::find_if(room->threads.begin(), room->threads.end(), [&](const auto& thread)
		{
			return (thread.uuid == uuid);
		});

		if (res != room->threads.end())
			return &*res;
	}

	return nullptr;
}

void RoomManager::OnNewThreadAddedToCurrentRoom(SimpleApoapseThread& thread, UIId uiId)
{
	JsonHelper ser;

	ser.Insert("internal_id", uiId);
	ser.Insert("name", HTMLUI::HtmlSpecialChars(thread.name, false));
	ser.Insert("lastMsgAuthor", thread.lastMessageAuthor.ToStr());
	ser.Insert("lastMsgText", thread.lastMessageText);

	global->htmlUI->SendSignal("on_added_new_thread", ser.Generate());
}

void RoomManager::LoadThreadsLists()
{
	for (auto& room : m_rooms)
	{
		SQLQuery query(*global->database);
		query << SELECT << ALL << FROM << "threads" << WHERE << "room_uuid" << EQUALS << room->uuid.GetInRawFormat() << ORDER_BY << "id" << ASC ;
		auto res = query.Exec();

		for (const auto& row : res)
		{
			SimpleApoapseThread thread;
			thread.dbId = row[0].GetInt64();
			thread.uuid = Uuid(row[2].GetByteArray());
			thread.name = row[3].GetText();
			thread.roomUuid = room->uuid;

			ASSERT(Uuid(row[1].GetByteArray()) == thread.roomUuid);

			ApoapseThread::UpdateThreadLastMessagePreview(thread, *this);
			room->threads.push_back(thread);
		}
	}
}

void RoomManager::UpdateThreadListUI() const
{
	if (m_uiSelectedRoom == nullptr)
		return;

	const auto* selectedRoom = GetSelectedRoom();
	JsonHelper ser;

	// Reverse loop
	for (size_t i = selectedRoom->threads.size(); i-- > 0;)
	{
		JsonHelper serThread;
		const auto& thread = selectedRoom->threads.at(i);

		serThread.Insert("internal_id", i);
		serThread.Insert("name", HTMLUI::HtmlSpecialChars(thread.name, false));
		serThread.Insert("lastMsgAuthor", HTMLUI::HtmlSpecialChars(User::GetUserByUsername(thread.lastMessageAuthor).nickname, false));
		serThread.Insert("lastMsgText", HTMLUI::HtmlSpecialChars(thread.lastMessageText, false));

		ser.Insert("threads", serThread);
	}

	global->htmlUI->SendSignal("threads_list_update", ser.Generate());
}

void RoomManager::UpdateUI() const
{
	// Rooms
	{
		JsonHelper ser;

		int i = 0;
		for (const auto& room : m_rooms)
		{
			JsonHelper serRoom;
			serRoom.Insert("internal_id", i);
			serRoom.Insert("name", HTMLUI::HtmlSpecialChars(room->name, false));

			if (room.get() == m_uiSelectedRoom)
				serRoom.Insert("isSelected", true);
			else
				serRoom.Insert("isSelected", false);


			ser.Insert("rooms", serRoom);
			i++;
		}

		global->htmlUI->SendSignal("rooms_update", ser.Generate());
	}
}
