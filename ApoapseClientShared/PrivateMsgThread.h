// ----------------------------------------------------------------------------
// Copyright (C) 2020 Apoapse
// Copyright (C) 2020 Guillaume Puyal
//
// Distributed under the Apoapse Pro Client Software License. Non-commercial use only.
// See accompanying file LICENSE.md
//
// For more information visit https://github.com/apoapse/
// And https://apoapse.space/
// ----------------------------------------------------------------------------

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
	std::string m_unsentMessage;
	
public:
	Int64 unreadMesagesCount = 0;
	
	PrivateMsgThread(const User& relatedUser, ContentManager& cManager);
	
	DbId relatedUserId = -1;
	const User* relatedUserPtr = nullptr;

	void RefreshUnreadMessagesCount();
	void LoadMessages(ContentManager& contentManager);
	void LoadNextMessagesChunk(Int64 messagesLoaded);
	void AddNewMessage(PrivateMessage& message);
	void SetUnsentMessage(const std::string& msgContent);
	
	JsonHelper GetJson() const;
	PrivateMessage& GetMessageById(DbId id);
	PrivateMessage* GetMessageByUuid(const Uuid& uuid);

private:
};