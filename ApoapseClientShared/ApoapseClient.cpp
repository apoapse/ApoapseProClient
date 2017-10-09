#include "stdafx.h"
#include "ApoapseClient.h"
#include "Common.h"
#include "Json.hpp"
#include "ClientConnection.h"
#include "HTMLUI.h"
#include "CmdConnect.h"

void ApoapseClient::Connect(const std::string& serverAddress, const std::string& username, const std::string& password)
{
	if (serverAddress.length() < 3)
	{
		global->htmlUI->UpdateStatusBar("@invalid_server_address", true);
		return;
	}
	else if ( username.length() < 6 || password.length() < 8)// #TODO use values from the create user cmd)
	{
		global->htmlUI->UpdateStatusBar("@invalid_login_input", true);
		return;
	}

	global->htmlUI->UpdateStatusBar("@password_encryption_status");
	CmdConnect connectCmd;
	LOG << "Starting login hashing";
	connectCmd.PrepareLoginCmd(username, password);
	LOG << "Login hashing complete";

	global->htmlUI->UpdateStatusBar("@connecting_status");
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

void ApoapseClient::OnDisconnect()
{
	m_connected = false;

	global->htmlUI->UpdateStatusBar("@disconnected_status", true);
}

void ApoapseClient::OnUILogin(const JsonHelper& deserializer)
{
	Connect(deserializer.ReadFieldValue<std::string>("server").get(), deserializer.ReadFieldValue<std::string>("server").get(), deserializer.ReadFieldValue<std::string>("password").get());
}
