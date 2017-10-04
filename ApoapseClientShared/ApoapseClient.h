#pragma once
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
	void OnDisconnect();
	void Login(const std::string& json);
private:
};