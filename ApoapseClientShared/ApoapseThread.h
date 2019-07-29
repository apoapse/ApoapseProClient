#pragma once
#include "TypeDefs.hpp"
#include "Uuid.h"
#include "DateTimeUtils.h"
class ContentManager;
struct Room;

struct ApoapseMessage
{
	DbId id = -1;
	Uuid uuid;
	Uuid threadUuid;
	//User author;
	std::string message;
	DateTimeUtils::UTCDateTime sentTime;

	ApoapseMessage(DataStructure& data);
	JsonHelper GetJson() const;
};

class ApoapseThread
{
	std::vector<ApoapseMessage> m_messages;

public:
	DbId id = -1;
	Uuid uuid;
	Uuid parrentRoomUuid;
	std::string name;
	ContentManager& contentManager;
	Room& parrentRoom;
	Int64 m_totalMessagesCount = -1;

	ApoapseThread(DataStructure& data, Room& parrentRoom, ContentManager& cManager);
	bool operator==(const ApoapseThread& other) const;

	JsonHelper GetJson() const;
	JsonHelper GetMessageListJson() const;

	void LoadMessages();
	void AddNewMessage(ApoapseMessage& message);

	static void LoadAllThreads(Room& room, ContentManager& cManager);

private:
	void LoadLastMessage();
};