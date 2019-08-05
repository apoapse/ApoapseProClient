#include "stdafx.h"
#include "Common.h"
#include "ApoapseThread.h"
#include "ContentManager.h"
#include "Json.hpp"
#include <SQLQuery.h>
#include "Username.h"
#include "HTMLUI.h"
#include "ApoapseClient.h"

ApoapseMessage::ApoapseMessage(DataStructure& data, ApoapseClient& client) : apoapseClient(client)
{
	id = data.GetDbId();
	uuid = data.GetField("uuid").GetValue<Uuid>();
	threadUuid = data.GetField("parent_thread").GetValue<Uuid>();
	message = data.GetField("message").GetValue<std::string>();
	sentTime = data.GetField("sent_time").GetValue<DateTimeUtils::UTCDateTime>();
	author = &apoapseClient.GetClientUsers().GetUserByUsername(data.GetField("author").GetValue<Username>());

	// tags
	{
		auto res = global->apoapseData->ReadListFromDatabase("tag", "item_uuid", uuid);
		tags.reserve(res.size());

		for (DataStructure& dat : res)
		{
			tags.push_back(dat.GetField("name").GetValue<std::string>());
		}
	}
}

JsonHelper ApoapseMessage::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("message", HTMLUI::HtmlSpecialChars(message));
	ser.Insert("sent_time", sentTime.GetStr());
	ser.Insert("author", author->nickname);
	ser.Insert("author_id", author->id);
	ser.InsertArray("tags", tags);

	return ser;
}

ApoapseThread::ApoapseThread(DataStructure& data, Room& parrentRoom, ContentManager& cManager)
	: contentManager(cManager)
	, parrentRoom(parrentRoom)
{
	id = data.GetDbId();
	uuid = data.GetField("uuid").GetValue<Uuid>();
	parrentRoomUuid = data.GetField("parent_room").GetValue<Uuid>();
	name = data.GetField("name").GetValue<std::string>();

	{
		SQLQuery query(*global->database);
		query << SELECT << "COUNT(*)" FROM << "messages" << WHERE << "parent_thread" << EQUALS << uuid.GetBytes();
		auto res = query.Exec();

		m_totalMessagesCount = res[0][0].GetInt64();
	}

	if (m_totalMessagesCount > 0)
	{
		LoadLastMessage();
	}

	ASSERT(parrentRoom.uuid == parrentRoomUuid);
}

bool ApoapseThread::operator==(const ApoapseThread& other) const
{
	return (other.uuid == uuid);
}

JsonHelper ApoapseThread::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("name", name);
	ser.Insert("msg_count", m_totalMessagesCount);

	if (!m_messages.empty())
	{
		auto& mostRecentMsg = m_messages.at(m_messages.size() - 1);
		ser.Insert("msg_preview", mostRecentMsg.GetJson());
	}

	return ser;
}

JsonHelper ApoapseThread::GetMessageListJson() const
{
	JsonHelper ser;
	for (const auto& message : m_messages)
	{
		ser.Insert("messages", message.GetJson());
	}

	return ser;
}

ApoapseMessage& ApoapseThread::GetMessageById(DbId id)
{
	auto res = std::find_if(m_messages.begin(), m_messages.end(), [id](ApoapseMessage& msg)
	{
		return (msg.id == id);
	});

	if (res == m_messages.end())
		throw std::exception("The message with the provided id do not exist on this thread");

	return *res;
}

ApoapseMessage* ApoapseThread::GetMessageByUuid(const Uuid& uuid)
{
	auto res = std::find_if(m_messages.begin(), m_messages.end(), [&uuid](ApoapseMessage& msg)
	{
		return (msg.uuid == uuid);
	});

	if (res != m_messages.end())
		return &*res;

	return nullptr;
}

void ApoapseThread::LoadMessages()
{
	if (m_messages.size() != m_totalMessagesCount)
	{
		m_messages.clear();
		m_messages.reserve(m_totalMessagesCount);

		auto messages = global->apoapseData->ReadListFromDatabase("message", "parent_thread", uuid);
		for (auto& messageDat : messages)
		{
			m_messages.push_back(ApoapseMessage(messageDat, contentManager.client));
		}
	}

	ASSERT(m_messages.size() == m_totalMessagesCount);
	LOG << "Loaded " << m_messages.size() << " messages on thread " << name;
}

void ApoapseThread::AddNewMessage(ApoapseMessage& message)
{
	m_totalMessagesCount++;
	m_messages.push_back(message);

	// User is on the threads list
	if (contentManager.GetCurrentRoom() == parrentRoom && !contentManager.IsThreadDisplayed())
	{
		global->htmlUI->SendSignal("UpdateThreadPreview", GetJson().Generate());
	}

	// User is reading the thread
	if (contentManager.GetCurrentRoom() == parrentRoom && contentManager.IsThreadDisplayed() && contentManager.GetCurrentThread() == *this)
	{
		global->htmlUI->SendSignal("NewMessage", message.GetJson().Generate());
	}
}

void ApoapseThread::LoadAllThreads(Room& room, ContentManager& cManager)
{
	if (room.threads.empty())	// We don't load treads if there are any already
	{
		auto threads = global->apoapseData->ReadListFromDatabase("thread", "parent_room", room.uuid);
		room.threads.reserve(threads.size());

		for (auto& threadData : threads)
		{
			room.threads.push_back(ApoapseThread(threadData, room, cManager));
		}

		LOG << "Loaded " << room.threads.size() << " threads on room " << room.name;
	}
}

void ApoapseThread::LoadLastMessage()
{
	auto messages = global->apoapseData->ReadListFromDatabase("message", "parent_thread", uuid, "id", ResultOrder::desc, 1);
	ASSERT(messages.size() == 1);

	for (auto& messageDat : messages)
	{
		m_messages.push_back(ApoapseMessage(messageDat, contentManager.client));
	}
}
