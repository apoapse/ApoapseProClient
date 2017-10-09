#pragma once
#include "WebResourcesManager.h"
#include <memory>
#include "ThreadPool.h"
#include "ISignalSender.hpp"
class ApoapseClient;

class HTMLUI
{
	ApoapseClient& m_apoapseClient;
	std::unique_ptr<WebResourcesManager> m_webResourcesManager;
	ISignalSender* m_uiSignalSender = nullptr;
	std::unique_ptr<ThreadPool> m_clientMainThread;

public:

	HTMLUI(ApoapseClient& client);

// 	virtual ~HTMLUI();

	WebResourcesManager& GetWebResourcesManager() const
	{
		return *m_webResourcesManager;
	}

	void RegisterSignalSender(ISignalSender* signalSender);

	std::string OnReceivedSignal(const std::string& name, const std::string& data);
	void SendSignal(const std::string& name, const std::string& data);
	void UpdateStatusBar(const std::string& msg, bool isError = false);
	
private:

};