#include "stdafx.h"
#include "ApoapseClient.h"
#include "Common.h"
#include "Json.hpp"
#include "ClientConnection.h"
#include "HTMLUI.h"

void ApoapseClient::Connect(const std::string& serverAddress, const std::string& username, const std::string& password)
{
	if (serverAddress.length() < 3)
	{
		global->htmlUI->UpdateStatusBar("@invalid_server_address", true);
		return;
	}
	else if ( username.length() < 6 || password.length() < 8)// #TODO use values from the create user cmd
	{
		global->htmlUI->UpdateStatusBar("@invalid_login_input", true);
		return;
	}

	global->htmlUI->UpdateStatusBar("@password_encryption_status");
	{
		m_loginCmd = std::make_unique<CmdConnect>();

		LOG << "Starting login hashing";
		m_loginCmd.value()->PrepareLoginCmd(username, password);
		LOG << "Login hashing complete";
	}

	{
		global->htmlUI->UpdateStatusBar("@connecting_status");
		const UInt16 port = 3000;
		auto connection = std::make_shared<ClientConnection>(m_IOService, *this);
		connection->Connect(serverAddress, port);

		m_connection = connection.get();
		LOG << "TCP Client started to " << serverAddress << " port: " << port;
	}

	std::thread threadMainClient([this]
	{
		m_IOService.run();
	});
	threadMainClient.detach();
}

std::string ApoapseClient::OnReceivedSignal(const std::string& name, const std::string& data)
{
	return "";
}

std::string ApoapseClient::OnReceivedSignal(const std::string& name, const JsonHelper& json)
{
	if (name == "login" && !m_connected)
	{
		OnUILogin(json);
	}

	return "";
}

void ApoapseClient::OnConnectedToServer()
{
	m_connected = true;
	global->htmlUI->UpdateStatusBar("@connected_waiting_authentication");

	m_loginCmd.value()->Send(*m_connection);
	m_loginCmd.reset();
}

void ApoapseClient::OnDisconnect(bool isAuthenticated)
{
	m_connected = false;

	global->htmlUI->UpdateStatusBar("@disconnected_status", true);

	if (!isAuthenticated)
		global->htmlUI->SendSignal("login_form_enable_back", "");
}

void ApoapseClient::OnUILogin(const JsonHelper& deserializer)
{
	Connect(deserializer.ReadFieldValue<std::string>("server").get(), deserializer.ReadFieldValue<std::string>("server").get(), deserializer.ReadFieldValue<std::string>("password").get());
}
