#pragma once
#include "Json.hpp"
class ClientConnection;

class ApoapseClient
{
	boost::asio::io_service m_IOService;
	ClientConnection* m_connection;
	bool m_connected;

public:
	//ApoapseClient();
	//virtual ~ApoapseClient();


	void Connect(const std::string& serverAddress, const std::string& username, const std::string& password);
	
	std::string OnReceivedSignal(const std::string& name, const std::string& data);
	std::string OnReceivedSignal(const std::string& name, const JsonHelper& deserializer);
	void OnDisconnect();

	void OnUILogin(const JsonHelper& deserializer);
private:
};