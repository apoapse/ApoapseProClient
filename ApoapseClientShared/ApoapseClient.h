#pragma once
#include "Json.hpp"
#include <optional>
#include "CmdConnect.h"
class ClientConnection;

class ApoapseClient
{
	boost::asio::io_service m_IOService;
	ClientConnection* m_connection;
	bool m_connected;
	std::optional<std::unique_ptr<CmdConnect>> m_loginCmd;

public:
	//ApoapseClient();
	//virtual ~ApoapseClient();

	ClientConnection* GetConnection() const;
	bool IsConnected() const;

	void Connect(const std::string& serverAddress, const std::string& username, const std::string& password);
	
	std::string OnReceivedSignal(const std::string& name, const std::string& data);
	std::string OnReceivedSignal(const std::string& name, const JsonHelper& deserializer);
	void OnConnectedToServer();
	void OnSetupState();
	void OnDisconnect(bool IsAuthenticated);
	

	void OnUILogin(const JsonHelper& deserializer);
private:
};