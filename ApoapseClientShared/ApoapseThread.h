#pragma once
#include "TypeDefs.hpp"
#include "Uuid.h"
#include "DateTimeUtils.h"
#include "Attachment.h"
class ContentManager;
class ApoapseClient;
class User;
struct Room;

struct ApoapseMessage
{
	ApoapseClient& apoapseClient;

	DbId id = -1;
	Uuid uuid;
	std::optional<Uuid> threadUuid;
	const User* author;
	std::string message;
	DateTimeUtils::UTCDateTime sentTime;
	std::vector<std::string> tags;
	bool isRead = true;
	std::vector<Attachment*> attachments;

	bool supportTags = true;

	ApoapseMessage(DataStructure& data, ApoapseClient& client);
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
	Int64 totalMessagesCount = 0;
	Int64 unreadMesagesCount = 0;

	ApoapseThread(DataStructure& data, Room& parrentRoom, ContentManager& cManager);
	bool operator==(const ApoapseThread& other) const;

	JsonHelper GetJson() const;
	JsonHelper GetMessageListJson() const;
	ApoapseMessage& GetMessageById(DbId id);
	ApoapseMessage* GetMessageByUuid(const Uuid& uuid);

	void LoadMessages();
	void AddNewMessage(ApoapseMessage& message);

	void RefreshUnreadMessagesCount();

	static void LoadAllThreads(Room& room, ContentManager& cManager);

private:
	void LoadLastMessage();
};