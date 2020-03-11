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
#include "HTMLUI.h"
#include "Common.h"
#include <Json.hpp>
#include "ApoapseClient.h"
#include <regex>

HTMLUI::HTMLUI(ApoapseClient& client) : m_apoapseClient(client)
{
	m_webResourcesManager = std::make_unique<WebResourcesManager>(client);
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

	auto res = global->mainThread->PushTaskFuture([this, name, data]
	{
		if (!data.empty() && data.at(0) == '{')
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

	//return res.get();
	return ""s;	// #TODO Temporary disabled return from signal to avoid stalls of the UI. In the future, add a specific "callback" json field to automatically callback the result.
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

//	SendSignal("update_status_bar", writer.Generate());
}

std::string HTMLUI::HtmlSpecialChars(const std::string& str, bool convertNewLines/* = false*/)	// #TODO make unit tests for this function
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
