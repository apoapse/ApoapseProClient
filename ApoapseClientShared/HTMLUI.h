#pragma once
#include "WebResourcesManager.h"
#include <memory>
#include "ISignalSender.hpp"

class HTMLUI
{
	std::unique_ptr<WebResourcesManager> m_webResourcesManager;
	ISignalSender* m_uiSignalSender = nullptr;

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

	void RegisterSignalSender(ISignalSender* signalSender);

	std::string OnReceivedSignal(const std::string& name, const std::string& data);
	void SendSignal(const std::string& name, const std::string& data);
	
private:

};