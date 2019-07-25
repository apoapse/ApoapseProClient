#pragma once
#include "TypeDefs.hpp"
#include "Uuid.h"
class ContentManager;
struct Room;

class ApoapseThread
{
public:
	DbId id = -1;
	Uuid uuid;
	Uuid parrentRoomUuid;
	std::string name;
	ContentManager& contentManager;
	Room& parrentRoom;

	ApoapseThread(DataStructure& data, Room& parrentRoom, ContentManager& cManager);

	JsonHelper GetJson() const;

	static void LoadAllThreads(Room& room, ContentManager& cManager);
};