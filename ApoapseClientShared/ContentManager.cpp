#include "stdafx.h"
#include "Common.h"
#include "ContentManager.h"
#include "Json.hpp"
#include "HTMLUI.h"
#include "ApoapseClient.h"
#include <numeric>

Room::Room(DataStructure& data)
{
	name = data.GetField("name").GetValue<std::string>();
	uuid = data.GetField("uuid").GetValue<Uuid>();
	threadsLayout = (data.GetField("threads_layout").GetValue<std::string>() == "single") ? ThreadsLayout::single : ThreadsLayout::multiple;
	id = data.GetDbId();
}

void Room::RefrechUnreadMessagesCount()
{
	unreadMsgCount = 0;

	for (auto& thread : threads)
	{
		thread->RefreshUnreadMessagesCount();
		unreadMsgCount += thread->unreadMesagesCount;
	}
}

bool Room::operator==(const Room& other) const
{
	return (id == other.id);
}

JsonHelper Room::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("name", HTMLUI::HtmlSpecialChars(name));
	ser.Insert("unreadMsgCount", unreadMsgCount);

	return ser;
}

ApoapseThread& Room::GetThread(DbId id)
{
	const auto res = std::find_if(threads.begin(), threads.end(), [id](const auto& thread)
	{
		return (thread->id == id);
	});

	if (res == threads.end())
		throw std::exception("Unable to find the thread with the provided id in this room");

	return **res;
}

ContentManager::ContentManager(ApoapseClient& apoapseClient) : client(apoapseClient)
{
	//global content cache system (nothing to do with signals)
}

void ContentManager::Init()
{
	// Loading rooms
	auto rooms = global->apoapseData->ReadListFromDatabase("room", "", "");
	for (auto& roomData : rooms)
	{
		auto room = std::make_unique<Room>(roomData);
		ApoapseThread::LoadAllThreads(*room, *this);
		room->RefrechUnreadMessagesCount();

		m_rooms.push_back(std::move(room));
	}

	// Open room if any
	if (!m_rooms.empty())
	{
		OpenRoom(*m_rooms[0]);
	}
}

void ContentManager::OnReceivedSignal(const std::string& name, const JsonHelper& json)
{
	if (name == "loadRoomUI")
	{
		Room& room = GetRoomById(json.ReadFieldValue<Int64>("id").value());
		OpenRoom(room);
	}

	else if (name == "loadThread")
	{
		ApoapseThread& thread = m_selectedRoom->GetThread(json.ReadFieldValue<Int64>("id").value());
		OpenThread(thread);
	}
}

void ContentManager::OnAddNewRoom(DataStructure& data)
{
	auto room = std::make_unique<Room>(data);
	LOG << "Added new room " << room->name << " id: " << room->id;

	m_rooms.push_back(std::move(room));

	if (m_rooms.size() == 1)
		OpenRoom(*m_rooms[0]);	//If this is the first room created, open it directly
	else
		UIRoomsUpdate();
}

void ContentManager::OnAddNewThread(DataStructure& data)
{
	auto& parentRoom = GetRoomByUuid(data.GetField("parent_room").GetValue<Uuid>());
	auto thread = std::make_unique<ApoapseThread>(data, parentRoom, *this);

	LOG << "Added new thread " << thread->name << " id: " << thread->id;

	if (parentRoom == *m_selectedRoom)
	{
		global->htmlUI->SendSignal("OnNewThreadOnCurrentRoom", thread->GetJson().Generate());
	}

	parentRoom.threads.push_back(std::move(thread));

	// If on the current room and it use a single thread layout, we open it directly
	if (m_selectedRoom && parentRoom == *m_selectedRoom && parentRoom.threadsLayout == Room::ThreadsLayout::single)
	{
		OpenThread(*parentRoom.threads.at(0));
	}
}

void ContentManager::OnAddNewMessage(DataStructure& data)
{
	auto message = ApoapseMessage(data, client);
	auto& parentThread = GetThreadByUuid(message.threadUuid);

	parentThread.AddNewMessage(message);
}

void ContentManager::OnAddNewTag(DataStructure& data)
{
	if (data.GetField("item_type").GetValue<std::string>() == "msg")
	{
		ApoapseThread* thread = nullptr;

		{
			auto dat = global->apoapseData->ReadItemFromDatabase("message", "uuid", data.GetField("item_uuid").GetValue<Uuid>());
			thread = &GetThreadByUuid(dat.GetField("parent_thread").GetValue<Uuid>());
		}
		auto* message = thread->GetMessageByUuid(data.GetField("item_uuid").GetValue<Uuid>());

		if (message)
		{
			const std::string tag = data.GetField("name").GetValue<std::string>();
			message->tags.push_back(tag);

			if (m_selectedThread && *m_selectedThread == *thread)
			{
				JsonHelper ser;
				ser.Insert("name", HTMLUI::HtmlSpecialChars(tag, true));
				ser.Insert("msgId", message->id);
				global->htmlUI->SendSignal("AddTag", ser.Generate());
			}
		}
	}
}

void ContentManager::MarkMessageAsRead(const Uuid& uuid)
{
	auto dat = global->apoapseData->ReadItemFromDatabase("message", "uuid", uuid);
	dat.GetField("is_read").SetValue(true);
	dat.SaveToDatabase();

	auto& parentThread = GetThreadByUuid(dat.GetField("parent_thread").GetValue<Uuid>());
	parentThread.unreadMesagesCount--;
	parentThread.parrentRoom.unreadMsgCount--;

	auto* msgObject = parentThread.GetMessageByUuid(dat.GetField("uuid").GetValue<Uuid>());
	if (msgObject)
	{
		msgObject->isRead = true;
	}

	UIRoomsUpdate();
}

Room& ContentManager::GetRoomById(DbId id)
{
	const auto res = std::find_if(m_rooms.begin(), m_rooms.end(), [id](const auto& room)
	{
		return (room->id == id);
	});
	
	if (res == m_rooms.end())
		throw std::exception("Unable to find the room with the provided id");

	return **res;
}

Room& ContentManager::GetRoomByUuid(const Uuid& uuid)
{
	const auto res = std::find_if(m_rooms.begin(), m_rooms.end(), [&uuid](const auto& room)
	{
		return (room->uuid == uuid);
	});

	if (res == m_rooms.end())
		throw std::exception("Unable to find the room with the provided uuid");

	return **res;
}

ApoapseThread& ContentManager::GetThreadByUuid(const Uuid& uuid)
{
	for (auto& room : m_rooms)
	{
		for (auto& thread : room->threads)
		{
			if (thread->uuid == uuid)
				return *thread;
		}
	}

	throw std::exception("The requested thread cannot be found");
}

ApoapseThread& ContentManager::GetThreadById(DbId id)
{
	for (auto& room : m_rooms)
	{
		for (auto& thread : room->threads)
		{
			if (thread->id == id)
				return *thread;
		}
	}

	throw std::exception("The requested thread cannot be found with the DbId provided");
}

void ContentManager::OpenRoom(Room& room)
{
	m_selectedRoom = &room;
	m_selectedThread = nullptr;

	LOG << "Selected room " << room.name;

	room.RefrechUnreadMessagesCount();
	UIRoomsUpdate();

	if (room.threadsLayout == Room::ThreadsLayout::multiple)
	{
		ApoapseThread::LoadAllThreads(*m_selectedRoom, *this);

		JsonHelper ser;
		for (const auto& thread : m_selectedRoom->threads)
		{
			ser.Insert("threads", thread->GetJson());
		}

		ser.Insert("room", room.GetJson());

		global->htmlUI->SendSignal("OnOpenRoom", ser.Generate());
	}
	else
	{
		if (!room.threads.empty())
		{
			OpenThread(*room.threads.at(0));
			LOG << "This room is of single thread type, the default thread was opened directly";
		}
	}
}

void ContentManager::OpenThread(ApoapseThread& thread)
{
	m_selectedThread = &thread;
	m_selectedThread->LoadMessages();

	JsonHelper ser = m_selectedThread->GetMessageListJson();
	ser.Insert("thread", thread.GetJson());

	global->htmlUI->SendSignal("OnOpenThread", ser.Generate());
}

Room& ContentManager::GetCurrentRoom()
{
	if (!m_selectedRoom)
		throw std::exception("There is no room currently selected");

	return *m_selectedRoom;
}

bool ContentManager::IsThreadDisplayed() const
{
	return (m_selectedThread != nullptr);
}

ApoapseThread& ContentManager::GetCurrentThread()
{
	if (!m_selectedThread)
		throw std::exception("No thread selected");

	return *m_selectedThread;
}

void ContentManager::UIRoomsUpdate() const
{
	JsonHelper ser;

	for (const auto& room : m_rooms)
	{
		auto json = room->GetJson();

		const bool isSelected = (m_selectedRoom && *room == *m_selectedRoom);
		json.Insert("is_selected", isSelected);

		ser.Insert("rooms", json);
	}

	global->htmlUI->SendSignal("rooms_update", ser.Generate());
}
/*
void ContentManager::UpdateUnreadMsgCount(const Room& room) const
{
	Int64 roomUnreadMsg = 0;
	JsonHelper ser;

	for (const auto& thread : room.threads)
	{
		const Int64 threadUnreadMsgs = thread->CountUnreadMessages();
		roomUnreadMsg += threadUnreadMsgs;

		JsonHelper threadSer;
		threadSer.Insert("id", thread->id);
		threadSer.Insert("unreadMsgCount", threadUnreadMsgs);

		ser.Insert("threads", threadSer);
	}

	ser.Insert("roomUnreadMsgCount", roomUnreadMsg);
	ser.Insert("roomId", room.id);

	global->htmlUI->SendSignal("UpdateUnreadMsgCount", ser.Generate());
}*/