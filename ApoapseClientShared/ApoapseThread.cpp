#include "stdafx.h"
#include "Common.h"
#include "ApoapseThread.h"
#include "ContentManager.h"
#include "Json.hpp"

ApoapseThread::ApoapseThread(DataStructure& data, Room& parrentRoom, ContentManager& cManager)
	: contentManager(cManager)
	, parrentRoom(parrentRoom)
{
	id = data.GetDbId();
	uuid = data.GetField("uuid").GetValue<Uuid>();
	parrentRoomUuid = data.GetField("parent_room").GetValue<Uuid>();
	name = data.GetField("name").GetValue<std::string>();

	ASSERT(parrentRoom.uuid == parrentRoomUuid);
}

JsonHelper ApoapseThread::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("name", name);

	return ser;
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
