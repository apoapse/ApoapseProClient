#include "stdafx.h"
#include "HTMLUI.h"
#include "Common.h"
#include <Json.hpp>
#include "ApoapseClient.h"

HTMLUI::HTMLUI(ApoapseClient& client) : m_apoapseClient(client)
{
	m_webResourcesManager = std::make_unique<WebResourcesManager>();
}

void HTMLUI::RegisterSignalSender(ISignalSender* signalSender)
{
	m_uiSignalSender = signalSender;
}

std::string HTMLUI::OnReceivedSignal(const std::string& name, const std::string& data)
{
	// Process internal signals
	if (name == "apoapse_log")
	{
		LOG << "[JS] " << data;

		return "";
	}

	LOG_DEBUG << "Received signal " << name << " data: " << data;

	return m_apoapseClient.OnReceivedSignal(name, data);
}

void HTMLUI::SendSignal(const std::string& name, const std::string& data)
{
	m_uiSignalSender->SendSignal(name, data);
}

void HTMLUI::UpdateStatusBar(const std::string& msg, bool isError /*= false*/)
{
	JsonHelper writer;
	writer.Insert<std::string>("msg", msg);
	writer.Insert<bool>("is_error", isError);

	SendSignal("update_status_bar", writer.Generate());
}
