#pragma once
#include "Uuid.h"
#include "Username.h"
#include "DateTimeUtils.h"
#include "Json.hpp"
struct SimpleApoapseThread;
class RoomManager;

class ApoapseMessage
{


public:
	DbId dbId = -1;
	Uuid uuid;
	SimpleApoapseThread& thread;	//  #TODO put thread as shared ptr?
	Username author;
	DateTimeUtils::UTCDateTime sentTime;
	std::string content;

	ApoapseMessage(SimpleApoapseThread& thread);

	JsonHelper GenerateJson(Int64 internalId) const;
	static void AddNewMessageFromServer(std::unique_ptr<ApoapseMessage> message, RoomManager& roomManager);
	static bool DoesMessageExist(const Uuid& uuid);
	//virtual ~ApoapseMessage();
	
private:
};