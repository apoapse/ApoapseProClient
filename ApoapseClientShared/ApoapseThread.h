#pragma once
#include "Username.h"
#include "Uuid.h"
#include "ApoapseMessage.h"
class RoomManager;

struct SimpleApoapseThread
{
	DbId dbId = -1;
	Uuid uuid;
	Uuid roomUuid;
	std::string name;
	Username lastMessageAuthor;
	std::string lastMessageText;

	virtual ~SimpleApoapseThread() = default;
};

class ApoapseThread : public SimpleApoapseThread
{
	std::vector<std::unique_ptr<ApoapseMessage>> m_messages;

public:
	RoomManager* roomManager;

	ApoapseThread(const SimpleApoapseThread& simpleThread, RoomManager* roomManager);
	void LoadMessages();
	void OnUIDisplay();
	void SendNewMessage(const std::string& content);
	void OnAddedNewMessageFromServer(std::unique_ptr<ApoapseMessage> message);

	ApoapseMessage* GetMostRecentMessage() const;
	static void UpdateThreadLastMessagePreview(SimpleApoapseThread& thread, RoomManager& roomManager);
// 	virtual ~ApoapseThread();
	
private:
	void UpdateMessagesListUI() const;
};