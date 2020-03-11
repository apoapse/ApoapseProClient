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

	bool IsAuthenticated() const override;
	virtual std::optional<Username> GetConnectedUser() const override;

private:
	bool OnConnectedToServer() override;
	void OnReceivedCommand(CommandV2& cmd) override;
	virtual bool OnReceivedError(const boost::system::error_code& error) override;
};