#pragma once
#include "WebResourcesManager.h"
#include <memory>

class HTMLUI
{
	std::unique_ptr<WebResourcesManager> m_webResourcesManager;

public:

	HTMLUI()
	{
		m_webResourcesManager = std::make_unique<WebResourcesManager>();
	}

// 	virtual ~HTMLUI();

	WebResourcesManager& GetWebResourcesManager() const
	{
		return *m_webResourcesManager;
	}

	std::string OnReceivedSignal(const std::string& name, const std::string& data);
	void SendSignal(const std::string& name, const std::string& data);
	
private:

};