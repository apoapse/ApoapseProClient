#pragma once
#include "WebResourcesManager.h"
#include <memory>
#include "ThreadPool.h"
#include "ISignalSender.hpp"
#include "IHtmlUI.hpp"
class ApoapseClient;

class HTMLUI : public IHtmlUI
{
	ApoapseClient& m_apoapseClient;
	std::unique_ptr<WebResourcesManager> m_webResourcesManager;
	ISignalSender* m_uiSignalSender = nullptr;

public:

	HTMLUI(ApoapseClient& client);

// 	virtual ~HTMLUI();

	WebResourcesManager& GetWebResourcesManager() const
	{
		return *m_webResourcesManager;
	}

	void RegisterSignalSender(ISignalSender* signalSender);

	std::string OnReceivedSignal(const std::string& name, const std::string& data);
	void SendSignal(const std::string& name, const std::string& data) override;
	void UpdateStatusBar(const std::string& msg, bool isError = false) override;

	static std::string HtmlSpecialChars(const std::string& str, bool convertNewLines = false);
	
private:

};