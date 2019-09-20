#include "stdafx.h"
#include "Common.h"
#include "ContentManager.h"
#include "Json.hpp"
#include "HTMLUI.h"
#include "ApoapseClient.h"
#include "PrivateMsgThread.h"
#include "SearchResult.h"

Room::Room(DataStructure& data)
{
	name = data.GetField("name").GetValue<std::string>();
	uuid = data.GetField("uuid").GetValue<Uuid>();
	threadsLayout = (data.GetField("threads_layout").GetValue<std::string>() == "single") ? ThreadsLayout::single : ThreadsLayout::multiple;
	id = data.GetDbId();
}

void Room::RefreshUnreadMessagesCount()
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

ApoapseThread& Room::GetThread(DbId dbId)
{
	const auto res = std::find_if(threads.begin(), threads.end(), [dbId](const auto& thread)
	{
		return (thread->id == dbId);
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
	// Load attachments
	{
		auto attachments = global->apoapseData->ReadListFromDatabase("attachment", "", "");
		for (DataStructure& dat : attachments)
		{
			auto attachmentPtr = std::make_shared<Attachment>(dat, client);
			m_attachmentsPool.push_back(attachmentPtr);
		}

		LOG << "Loaded " << m_attachmentsPool.size() << " attachments";
	}
	
	// Load rooms
	{
		auto rooms = global->apoapseData->ReadListFromDatabase("room", "", "");
		for (auto& roomData : rooms)
		{
			auto room = std::make_unique<Room>(roomData);
			ApoapseThread::LoadAllThreads(*room, *this);
			room->RefreshUnreadMessagesCount();

			m_rooms.push_back(std::move(room));
		}
	}

	// Open room if any
	if (!m_rooms.empty())
	{
		OpenRoom(*m_rooms[0]);
	}
	
	// Load private messages threads
	{
		m_privateMsgThreads.reserve(std::max((Int64)client.GetClientUsers().GetUserCount() - 1, (Int64)1));	// -1 because we ignore the current user
		
		for (const User* user : client.GetClientUsers().GetUsers())
		{
			RegisterPrivateMsgThread(*user);
		}

		// UIUserListUpdate(); do not update here as it it already updated when receiving the server_info cmd
	}

	UpdateAttachmentsUI();
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

	else if (name == "LoadUserPage")
	{
		const User& user = client.GetClientUsers().GetUserById(json.ReadFieldValue<Int64>("id").value());
		if (user.username != client.GetLocalUser().username)
		{
			OpenPrivateMsgThread(*GetPrivateThreadByUserId(user.id));
		}
	}

	else if (name == "search")
	{
		PreSwitchPage();
		m_selectedRoom = nullptr;
		
		SearchResult res(json.ReadFieldValue<std::string>("query").value(), *this);
		global->htmlUI->SendSignal("DisplaySearchResults", res.GetJson().Generate());
	}

	else if (name == "SaveUnsentMessage")
	{
		if (json.ValueExist("threadId"))
		{
			const DbId roomId = json.ReadFieldValue<DbId>("roomId").value();
			const DbId threadId = json.ReadFieldValue<DbId>("threadId").value();
			auto& thread = GetRoomById(roomId).GetThread(threadId);

			thread.SetUnsentMessage(json.ReadFieldValue<std::string>("msgContent").value());
		}
		else
		{
			const DbId userId = json.ReadFieldValue<DbId>("userId").value();
			auto* privateThread = GetPrivateThreadByUserId(userId);

			privateThread->SetUnsentMessage(json.ReadFieldValue<std::string>("msgContent").value());
		}
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
	auto& parentThread = GetThreadByUuid(message.threadUuid.value());

	parentThread.AddNewMessage(message);
}

void ContentManager::OnAddNewPrivateMessage(DataStructure& data)
{
	auto message = PrivateMessage(data, client);
	auto* thread = GetPrivateThreadByUserId(message.author->id);
	if (thread)
		thread->AddNewMessage(message);

	if (IsUserPageDisplayed() && GetCurrentUserPage().relatedUserId == message.relatedUser->id)
	{
		global->htmlUI->SendSignal("NewMessage", message.GetJson().Generate());
	}
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
				ser.Insert("name", HTMLUI::HtmlSpecialChars(tag));
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

	if (dat.GetField("direct_recipient").HasValue())
	{
		const Username recipient = dat.GetField("direct_recipient").GetValue<Username>();
		const User* relatedUser = nullptr;
		
		if (recipient == client.GetLocalUser().username)
			relatedUser = &client.GetClientUsers().GetUserByUsername(dat.GetField("author").GetValue<Username>());
		else
			relatedUser = &client.GetClientUsers().GetUserByUsername(recipient);

		PrivateMsgThread* privateThread = GetPrivateThreadByUserId(relatedUser->id);
		privateThread->unreadMesagesCount--;

		UIUserListUpdate();

		/*if (IsUserPageDisplayed() && GetCurrentUserPage().relatedUserId == relatedUser->id)
		{
			
		}*/
	}
	else
	{
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

PrivateMsgThread* ContentManager::GetPrivateThreadByUserId(DbId id)
{
	const auto res = std::find_if(m_privateMsgThreads.begin(), m_privateMsgThreads.end(), [id](const auto& thread)
	{
		return (thread->relatedUserId == id);
	});

	if (res == m_privateMsgThreads.end())
		return nullptr;
	
	return &**res;
}

std::shared_ptr<Attachment> ContentManager::GetAttachment(const Uuid& uuid)
{
	const auto res = std::find_if(m_attachmentsPool.begin(), m_attachmentsPool.end(), [&uuid](std::shared_ptr<Attachment>& attachment)
	{
		return (attachment->relatedFile.uuid == uuid);
	});

	return *res;
}

std::shared_ptr<Attachment> ContentManager::GetAttachment(DbId id)
{
	const auto res = std::find_if(m_attachmentsPool.begin(), m_attachmentsPool.end(), [&id](std::shared_ptr<Attachment>& attachment)
	{
		return (attachment->id == id);
	});

	return *res;
}

void ContentManager::RegisterAttachment(std::shared_ptr<Attachment>& attachment)
{
	m_attachmentsPool.push_back(attachment);

	UpdateAttachmentsUI();
}

UInt64 ContentManager::GetAttachmentsCount() const
{
	return m_attachmentsPool.size();
}

void ContentManager::UpdateAttachmentsUI()
{
	JsonHelper ser;

	std::sort(m_attachmentsPool.begin(), m_attachmentsPool.end(), [](std::shared_ptr<Attachment> attA, std::shared_ptr<Attachment> attB)
	{
		return (attA->sentTime > attB->sentTime);
	});

	for (const auto& attachment : m_attachmentsPool)
	{
		ser.Insert("attachments", attachment->GetJson());
	}

	global->htmlUI->SendSignal("UpdateAttachments", ser.Generate());
}

void ContentManager::OpenRoom(Room& room)
{
	PreSwitchPage();
	m_selectedRoom = &room;

	LOG << "Selected room " << room.name;

	room.RefreshUnreadMessagesCount();

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
		else
		{
			ASSERT(false);
		}
	}

	UIRoomsUpdate();
	UIUserListUpdate();
}

void ContentManager::OpenThread(ApoapseThread& thread)
{
	PreSwitchPage();
	m_selectedThread = &thread;
	m_selectedThread->LoadMessages();

	JsonHelper ser = m_selectedThread->GetMessageListJson();
	ser.Insert("thread", thread.GetJson());

	global->htmlUI->SendSignal("OnOpenThread", ser.Generate());
}

void ContentManager::OpenPrivateMsgThread(PrivateMsgThread& thread)
{
	PreSwitchPage();
	m_selectedRoom = nullptr;
	m_selectedUserPage = &thread;

	thread.LoadMessages(*this);
	
	global->htmlUI->SendSignal("OnOpenPrivateMsgThread", thread.GetJson().Generate());

	UIRoomsUpdate();
	UIUserListUpdate();
}

void ContentManager::PreSwitchPage()
{
	m_selectedThread = nullptr;
	m_selectedUserPage = nullptr;
}

void ContentManager::RegisterPrivateMsgThread(const User& user)
{
	if (user.username == client.GetLocalUser().username)
		return;
	
	auto threadPtr = std::make_unique<PrivateMsgThread>(user, *this);
	m_privateMsgThreads.push_back(std::move(threadPtr));

	UIUserListUpdate();
}

Room& ContentManager::GetCurrentRoom() const
{
	if (!m_selectedRoom)
		throw std::exception("There is no room currently selected");

	return *m_selectedRoom;
}

bool ContentManager::IsThreadDisplayed() const
{
	return (m_selectedThread != nullptr);
}

ApoapseThread& ContentManager::GetCurrentThread() const
{
	if (!m_selectedThread)
		throw std::exception("No thread selected");

	return *m_selectedThread;
}

bool ContentManager::IsUserPageDisplayed() const
{
	return (m_selectedUserPage != nullptr);
}

PrivateMsgThread& ContentManager::GetCurrentUserPage() const
{
	if (!m_selectedUserPage)
		throw std::exception("No user page/private msg thread selected");

	return *m_selectedUserPage;
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

void ContentManager::UIUserListUpdate()
{
	JsonHelper ser;

	for (const User* user : client.GetClientUsers().GetUsers())
	{
		{
			JsonHelper serUser;
			serUser.Insert("id", user->id);
			serUser.Insert("nickname", HTMLUI::HtmlSpecialChars(user->nickname));
			serUser.Insert("isOnline", (user->GetStatus() == User::UserStatus::online));
			serUser.Insert("isLocalUser", user->isLocalUser);

			if (IsUserPageDisplayed())
				serUser.Insert("isSelected", (GetCurrentUserPage().relatedUserId == user->id));
			else
				serUser.Insert("isSelected", false);

			if (!user->isLocalUser)
			{
				auto* privateMsgThread = GetPrivateThreadByUserId(user->id);
				
				if (privateMsgThread)
					serUser.Insert("unreadMsgCount", privateMsgThread->unreadMesagesCount);
			}
			
			ser.Insert("users", serUser);
		}
	}
	
	global->htmlUI->SendSignal("OnUpdateUserList", ser.Generate());
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
