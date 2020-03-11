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

#include "stdafx.h"
#include "Common.h"
#include "PrivateMsgThread.h"
#include "ApoapseThread.h"
#include "ContentManager.h"
#include "Json.hpp"
#include "ApoapseClient.h"
#include <SQLQuery.h>
#include <boost/range/adaptor/reversed.hpp>

PrivateMessage::PrivateMessage(DataStructure& data, ApoapseClient& client) : ApoapseMessage(data, client)
{
	supportTags = false;
	
	const Username recipient = data.GetField("direct_recipient").GetValue<Username>();
	
	if (recipient == client.GetLocalUser().username)
		relatedUser = &client.GetClientUsers().GetUserByUsername(data.GetField("author").GetValue<Username>());
	else
		relatedUser = &client.GetClientUsers().GetUserByUsername(recipient);
}

PrivateMsgThread::PrivateMsgThread(const User& relatedUser, ContentManager& cManager) : contentManager(cManager)
{
	relatedUserId = relatedUser.id;
	relatedUserPtr = &relatedUser;

	RefreshUnreadMessagesCount();
}

void PrivateMsgThread::RefreshUnreadMessagesCount()
{
	if (m_messages.empty())
	{
		SQLQuery query(*global->database);
		query << SELECT << "COUNT(*)" FROM << "messages" << WHERE << "is_read" << EQUALS << 0;
		query << AND "(" << "direct_recipient" << EQUALS << relatedUserPtr->username.GetBytes();
		query << OR << "(author" << EQUALS << relatedUserPtr->username.GetBytes() << AND << "direct_recipient" << EQUALS << contentManager.client.GetLocalUser().username.GetBytes() << "))";
		auto res = query.Exec();

		unreadMesagesCount = res[0][0].GetInt64();
	}
	else
	{
		unreadMesagesCount = std::count_if(m_messages.begin(), m_messages.end(), [](const PrivateMessage& msg)
		{
			return (!msg.isRead);
		});
	}
}

void PrivateMsgThread::LoadMessages(ContentManager& contentManager)
{
	if (!m_messages.empty())
		m_messages.clear();
	
	const Username username = relatedUserPtr->username;

	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "messages" << WHERE << "direct_recipient" << EQUALS << username.GetBytes()
	<< OR << "(author" << EQUALS << username.GetBytes()
	<< AND << "direct_recipient" << EQUALS << contentManager.client.GetLocalUser().username.GetBytes() << ")"
	<< ORDER_BY << "id" << ASC;
	auto res = query.Exec();

	for (const auto& row : res)
	{
		auto datDef = global->apoapseData->GetStructure("message");
		DataStructure msgData = global->apoapseData->ReadFromDbResult(datDef, row);

		m_messages.push_back(PrivateMessage(msgData, contentManager.client));
	}

	LOG << "Loaded " << m_messages.size() << " private messages on user thread " << relatedUserPtr->nickname;
}

void PrivateMsgThread::LoadNextMessagesChunk(Int64 messagesLoaded)
{
	ASSERT(contentManager.IsUserPageDisplayed());
	if (messagesLoaded >= m_messages.size())
		return;

	JsonHelper ser;
	ser.Insert("totalMsgCount", m_messages.size());

	auto chunkMessages = Range(m_messages, m_messages.size() - messagesLoaded);
	const Int64 msgInThisChunk = std::min((Int64)ApoapseThread::maxMessagesPerChunk, (Int64)chunkMessages.size());
	chunkMessages.Consume(chunkMessages.size() - msgInThisChunk);

	for (auto& message : chunkMessages)
	{
		ser.Insert("messages", message.GetJson());
	}

	global->htmlUI->SendSignal("OnMessagesChunkLoaded", ser.Generate());
}

JsonHelper PrivateMsgThread::GetJson() const
{
	JsonHelper ser;
	ser.Insert("user.nickname", relatedUserPtr->nickname);
	ser.Insert("user.id", relatedUserPtr->id);
	ser.Insert("user.isOnline", (relatedUserPtr->GetStatus() == User::UserStatus::online));
	ser.Insert("unsentMessage", m_unsentMessage);
	ser.Insert("totalMsgCount", m_messages.size());

	// Messages
	{
		auto chunkMessages = Range(m_messages);

		const Int64 msgInThisChunk = std::min((Int64)ApoapseThread::maxMessagesPerChunk, (Int64)chunkMessages.size());
		chunkMessages.Consume(chunkMessages.size() - msgInThisChunk);

		for (auto& message : chunkMessages)
		{
			ser.Insert("messages", message.GetJson());
		}
	}

	return ser;
}

PrivateMessage& PrivateMsgThread::GetMessageById(DbId id)
{
	const auto res = std::find_if(m_messages.begin(), m_messages.end(), [id](ApoapseMessage& msg)
	{
		return (msg.id == id);
	});

	if (res == m_messages.end())
		throw std::exception("The private message with the provided id do not exist on this thread");

	return *res;
}

PrivateMessage* PrivateMsgThread::GetMessageByUuid(const Uuid& uuid)
{
	const auto res = std::find_if(m_messages.begin(), m_messages.end(), [&uuid](ApoapseMessage& msg)
	{
		return (msg.uuid == uuid);
	});

	if (res == m_messages.end())
		return nullptr;

	return &*res;
}

void PrivateMsgThread::AddNewMessage(PrivateMessage& message)
{
	if (!m_messages.empty())
	{
		m_messages.push_back(message);	// We add the message only if there are already because they will all be reloaded when the thread is opened
	}

	RefreshUnreadMessagesCount();
}

void PrivateMsgThread::SetUnsentMessage(const std::string& msgContent)
{
	if (msgContent.empty())
		return;
	
	m_unsentMessage = msgContent;
}
