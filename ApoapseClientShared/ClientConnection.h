#pragma once
#include "GenericConnection.h"
#include <optional>

class ApoapseClient;

class ClientConnection : public GenericConnection
{
public:
	ApoapseClient& client;

	ClientConnection(boost::asio::io_service& ioService, ssl::context& context, ApoapseClient& client);
	virtual ~ClientConnection() override;

private:
	bool OnConnectedToServer() override;
	void OnReceivedValidCommand(CommandV2& cmd) override;
	virtual bool OnReceivedError(const boost::system::error_code& error) override;
	bool IsAuthenticated() const;
};