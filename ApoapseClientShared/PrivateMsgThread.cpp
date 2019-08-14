#include "stdafx.h"
#include "Common.h"
#include "PrivateMsgThread.h"
#include "ApoapseThread.h"
#include "ContentManager.h"
#include "Json.hpp"
#include "ApoapseClient.h"
#include <SQLQuery.h>

PrivateMessage::PrivateMessage(DataStructure& data, ApoapseClient& client) : ApoapseMessage(data, client)
{
	supportTags = false;
	
	const Username recipient = data.GetField("direct_recipient").GetValue<Username>();
	
	if (recipient == client.GetLocalUser().username)
		relatedUser = &client.GetClientUsers().GetUserByUsername(data.GetField("author").GetValue<Username>());
	else
		relatedUser = &client.GetClientUsers().GetUserByUsername(recipient);
}

PrivateMsgThread::PrivateMsgThread(const User& relatedUser)
{
	relatedUserId = relatedUser.id;
	relatedUserPtr = &relatedUser;
}

void PrivateMsgThread::LoadMessages(ContentManager& contentManager)
{
	if (!m_messages.empty())
		m_messages.clear();
	
	const Username username = relatedUserPtr->username;

	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "messages" << WHERE << "direct_recipient" << EQUALS << username.GetBytes() << OR << "(author" << EQUALS << username.GetBytes() << AND << "direct_recipient" << EQUALS << contentManager.client.GetLocalUser().username.GetBytes() << ")";
	auto res = query.Exec();

	for (const auto& row : res)
	{
		auto datDef = global->apoapseData->GetStructure("message");
		DataStructure msgData = global->apoapseData->ReadFromDbResult(datDef, row);

		m_messages.push_back(PrivateMessage(msgData, contentManager.client));
	}

	auto size = res.RowCount();
	LOG << "Loaded " << m_messages.size() << " private messages on user thread " << relatedUserPtr->nickname;
}

JsonHelper PrivateMsgThread::GetJson() const
{
	JsonHelper ser;
	ser.Insert("user.nickname", relatedUserPtr->nickname);
	ser.Insert("user.isOnline", relatedUserPtr->isOnline);
	
	for (const auto& message : m_messages)
	{
		ser.Insert("messages", message.GetJson());
	}

	return ser;
}

void PrivateMsgThread::AddNewMessage(PrivateMessage& message)
{
	if (!m_messages.empty())
	{
		m_messages.push_back(message);	// We add the message only if there are already because they will all be reloaded when the thread is opened
	}
}