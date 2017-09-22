#include "stdafx.h"
#include "HTMLUI.h"
#include "Common.h"

void HTMLUI::RegisterSignalSender(ISignalSender* signalSender)
{
	m_uiSignalSender = signalSender;
}

std::string HTMLUI::OnReceivedSignal(const std::string& name, const std::string& data)
{
	std::string output{};

	// Process internal signals
	if (name == "apoapse_log")
	{
		LOG << "[JS] " << data;
		return output;
	}

	LOG_DEBUG << "Received signal " << name << " data: " << data;

	return output;
}

void HTMLUI::SendSignal(const std::string& name, const std::string& data)
{
	m_uiSignalSender->SendSignal(name, data);
}
