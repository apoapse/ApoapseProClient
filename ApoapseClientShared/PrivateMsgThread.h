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
	std::vector<PrivateMessage> m_messages;
	
public:
	PrivateMsgThread(const User& relatedUser);
	
	DbId relatedUserId = -1;
	const User* relatedUserPtr = nullptr;

	void LoadMessages(ContentManager& contentManager);
	JsonHelper GetJson() const;
	void AddNewMessage(PrivateMessage& message);
};