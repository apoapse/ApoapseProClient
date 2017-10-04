#include "stdafx.h"
#include "ApoapseClient.h"
#include "Common.h"
#include "Json.hpp"
#include "ClientConnection.h"
#include "HTMLUI.h"

void ApoapseClient::Connect(const std::string& serverAddress, const std::string& username, const std::string& password)
{
	const UInt16 port = 3000;
	auto connection = std::make_shared<ClientConnection>(m_IOService, *this);
	connection->Connect(serverAddress, port);

	m_connection = connection.get();
	m_connected = true;

	LOG << "TCP Client started to " << serverAddress << " port: " << port;

	std::thread threadMainClient([this]
	{
		m_IOService.run();
	});
	threadMainClient.detach();
}

std::string ApoapseClient::OnReceivedSignal(const std::string& name, const std::string& data)
{
	if (name == "login" && m_connected)
	{
		Login(data);
	}

	return "";
}

void ApoapseClient::OnDisconnect()
{
	ASSERT(m_connected);
	m_connected = false;

	global->htmlUI->UpdateStatusBar("@disconnected_status", true);
}

void ApoapseClient::Login(const std::string& json)
{
	JsonHelper deserializer(json);
	Connect(deserializer.ReadFieldValue<std::string>("server").get(), deserializer.ReadFieldValue<std::string>("server").get(), deserializer.ReadFieldValue<std::string>("password").get());
}
