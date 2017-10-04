#pragma once
#include "GenericConnection.h"
class ApoapseClient;

class ClientConnection : public GenericConnection
{
	ApoapseClient& client;
	bool m_isAuthenticated = false;

public:
	ClientConnection(boost::asio::io_service& ioService, ApoapseClient& client);
	virtual ~ClientConnection() override;

	virtual bool OnReceivedError(const boost::system::error_code& error) override;
	bool IsAuthenticated() const;
	void Authenticate();

private:
	bool OnConnectedToServer() override;
	void OnReceivedValidCommand(std::unique_ptr<Command> cmd) override;


};