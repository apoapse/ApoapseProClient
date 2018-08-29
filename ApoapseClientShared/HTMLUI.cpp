#include "stdafx.h"
#include "HTMLUI.h"
#include "Common.h"
#include <Json.hpp>
#include "ApoapseClient.h"
#include <regex>

HTMLUI::HTMLUI(ApoapseClient& client) : m_apoapseClient(client)
{
	m_webResourcesManager = std::make_unique<WebResourcesManager>();
	m_clientMainThread = std::make_unique<ThreadPool>("Client main thread", 1);
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

	auto res = m_clientMainThread->PushTask([this, name, data]
	{
		if (!data.empty() && data.substr(0, 1) == "{")
		{	
			// Data is in json
			JsonHelper jsonDeserializer(data);
			std::string test = m_apoapseClient.OnReceivedSignal(name, jsonDeserializer);
			return test;
		}
		else
		{
			JsonHelper jsonDeserializer("{}");
			std::string test = m_apoapseClient.OnReceivedSignal(name, jsonDeserializer);
			return test;
		}
		
	});

	return res.get();
	//return ""s;	// #TODO Temporary disabled return from signal to avoid stalls of the UI. In the future, add a specific "callback" json field to automatically callback the result.
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

std::string HTMLUI::HtmlSpecialChars(const std::string& str, bool convertNewLines)	// #TODO make unit tests for this function
{
	std::string output = str;

	output = std::regex_replace(output, std::regex("&"), "&amp;");
	output = std::regex_replace(output, std::regex("\""), "&quot;");
	output = std::regex_replace(output, std::regex("'"), "&apos;");
	output = std::regex_replace(output, std::regex("<"), "&lt;");
	output = std::regex_replace(output, std::regex(">"), "&gt;");

	if (convertNewLines)
	{
		output = std::regex_replace(output, std::regex("\\n"), "<br/>");
		output = std::regex_replace(output, std::regex("\\r\\n"), "<br/>");
	}
	else
	{
		output = std::regex_replace(output, std::regex("\\n"), " ");
		output = std::regex_replace(output, std::regex("\\r\\n"), " ");
	}

	output = std::regex_replace(output, std::regex("\t"), "");
	output = std::regex_replace(output, std::regex("\\\\"), "&bsol;");

	return output;
}
