#pragma once
#include "Uuid.h"
#include "Username.h"
#include "DateTimeUtils.h"
#include "Json.hpp"
struct SimpleApoapseThread;
class RoomManager;
class ApoapseClient;

enum class ReadStatus
{
	marked_as_read,
	marked_as_unread,
};

class ApoapseMessage
{


public:
	DbId dbId = -1;
	Uuid uuid;
	SimpleApoapseThread& thread;	//  #TODO put thread as shared ptr?
	Username author;
	DateTimeUtils::UTCDateTime sentTime;
	std::string content;
	bool isRead = true;

	ApoapseMessage(SimpleApoapseThread& thread);

	static void MarkMessageAsReadFromServer(const Uuid& uuid, ApoapseClient& client);

	static ApoapseMessage GetMessageByUuid(const Uuid& uuid, RoomManager& roomManager);
	void OnChangedReadStatus(ReadStatus readStatus, RoomManager& roomManager) const;

	JsonHelper GenerateJson(Int64 internalId) const;
	static void AddNewMessageFromServer(std::unique_ptr<ApoapseMessage> message, RoomManager& roomManager);
	static bool DoesMessageExist(const Uuid& uuid);
	//virtual ~ApoapseMessage();
	
private:
};