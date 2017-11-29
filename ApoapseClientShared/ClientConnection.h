#pragma once
#include "GenericConnection.h"
#include <optional>

class ApoapseClient;

class ClientConnection : public GenericConnection
{
	bool m_isAuthenticated = false;

public:
	ApoapseClient& client;

	ClientConnection(boost::asio::io_service& ioService, ApoapseClient& client);
	virtual ~ClientConnection() override;

	bool IsAuthenticated() const;
	void Authenticate();

private:
	bool OnConnectedToServer() override;
	void OnReceivedValidCommand(std::unique_ptr<Command> cmd) override;
	virtual bool OnReceivedError(const boost::system::error_code& error) override;

};