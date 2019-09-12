#pragma once
#include "ApoapseThread.h"
class ContentManager;

class SearchResult
{
	ContentManager& contentManager;
	std::string m_searchQuery;

	std::vector<ApoapseMessage> m_messages;
	std::vector<ApoapseThread> m_threads;
	std::vector<Attachment> m_attachments;
	
public:
	SearchResult(const std::string& query, ContentManager& cManager);

	JsonHelper GetJson() const;
	std::string GetSearchQuery() const;
	
private:
	void SearchFromTag();
	void SearchMessages();
	void SearchThreads();
	void SearchAttachments();
};