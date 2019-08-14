#pragma once
#include "TypeDefs.hpp"
#include "User.h"
#include "ApoapseThread.h"
class ContentManager;

struct PrivateMessage : public ApoapseMessage
{
	const User* relatedUser = nullptr;
	
	PrivateMessage(DataStructure& data, ApoapseClient& client);
};

class PrivateMsgThread
{
	ContentManager& contentManager;
	std::vector<PrivateMessage> m_messages;
	
public:
	Int64 unreadMesagesCount = 0;
	
	PrivateMsgThread(const User& relatedUser, ContentManager& cManager);
	
	DbId relatedUserId = -1;
	const User* relatedUserPtr = nullptr;

	void RefreshUnreadMessagesCount();
	void LoadMessages(ContentManager& contentManager);
	void AddNewMessage(PrivateMessage& message);
	
	JsonHelper GetJson() const;
	PrivateMessage& GetMessageById(DbId id);
};