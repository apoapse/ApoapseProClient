#include "stdafx.h"
#include "Common.h"
#include "SearchResult.h"
#include "ContentManager.h"
#include "SQLQuery.h"

SearchResult::SearchResult(const std::string& query, ContentManager& cManager) : m_searchQuery(query), contentManager(cManager)
{
	LOG << "Start search";
	LOG_DEBUG << m_searchQuery;

	if (m_searchQuery.substr(0, 1) == "#")
	{
		SearchFromTag();
	}
	else
	{
		SearchAttachments();
		SearchThreads();
		SearchMessages();
	}
}

JsonHelper SearchResult::GetJson() const
{
	JsonHelper ser;
	ser.Insert("query", GetSearchQuery());

	for (const auto& message : m_messages)
	{
		ser.Insert("messages", message.GetJson());
	}

	for (const auto& thread : m_threads)
	{
		ser.Insert("threads", thread.GetJson());
	}

	for (const auto& attachment : m_attachments)
	{
		ser.Insert("attachments", attachment.GetJson());
	}
	
	return ser;
}

std::string SearchResult::GetSearchQuery() const
{
	return m_searchQuery;
}

void SearchResult::SearchFromTag()
{
	const std::string searchPattern = m_searchQuery.substr(1, m_searchQuery.length()) + '%';
	DataStructureDef def = global->apoapseData->GetStructure("tag");
	
	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "tags" << WHERE << "item_type" << EQUALS << "msg"s << AND << "name" << LIKE << searchPattern;
	auto res = query.Exec();
	
	m_messages.reserve(res.RowCount());
	
	for (auto& row : res)
	{
		DataStructure tagItem = global->apoapseData->ReadFromDbResult(def, row);
		
		const Uuid msgUuid = tagItem.GetField("item_uuid").GetValue<Uuid>();
		DataStructure msgDat = global->apoapseData->ReadItemFromDatabase("message", "uuid", msgUuid);

		m_messages.push_back(ApoapseMessage(msgDat, contentManager.client));
	}

	LOG << "Found " << m_messages.size() << " messages using tag";
}

void SearchResult::SearchMessages()
{
	const std::string searchPattern = '%' + m_searchQuery + '%';
	DataStructureDef def = global->apoapseData->GetStructure("message");
	
	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "messages" << WHERE << "message" << LIKE << searchPattern;
	auto res = query.Exec();

	m_messages.reserve(res.RowCount());

	for (const auto& sqlRow : res)
	{
		DataStructure dat = global->apoapseData->ReadFromDbResult(def, sqlRow);

		m_messages.push_back(ApoapseMessage(dat, contentManager.client));
	}

	LOG << "Found " << m_messages.size() << " messages";
}

void SearchResult::SearchThreads()
{
	const std::string searchPattern = '%' + m_searchQuery + '%';
	DataStructureDef def = global->apoapseData->GetStructure("thread");
	
	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "threads" << WHERE << "name" << LIKE << searchPattern;
	auto res = query.Exec();

	m_threads.reserve(res.RowCount());

	for (const auto& sqlRow : res)
	{
		DataStructure dat = global->apoapseData->ReadFromDbResult(def, sqlRow);
		Room& parentRoom = contentManager.GetRoomByUuid(dat.GetField("parent_room").GetValue<Uuid>());
		
		m_threads.push_back(ApoapseThread(dat, parentRoom, contentManager));
	}

	LOG << "Found " << m_threads.size() << " threads";
}

void SearchResult::SearchAttachments()
{
	const std::string searchPattern = '%' + m_searchQuery + '%';
	DataStructureDef def = global->apoapseData->GetStructure("attachment");
	
	SQLQuery query(*global->database);
	query << SELECT << ALL << FROM << "attachments" << WHERE << "name" << LIKE << searchPattern;
	auto res = query.Exec();

	m_attachments.reserve(res.RowCount());

	for (const auto& sqlRow : res)
	{
		DataStructure dat = global->apoapseData->ReadFromDbResult(def, sqlRow);
		
		m_attachments.push_back(Attachment(dat, contentManager.client));
	}

	LOG << "Found " << m_attachments.size() << " attachments";
}
