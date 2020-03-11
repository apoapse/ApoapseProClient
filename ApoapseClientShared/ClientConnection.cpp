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

#include "stdafx.h"
#include "ClientConnection.h"
#include "Common.h"
#include "ApoapseClient.h"
#include "HTMLUI.h"
#include "ApoapseError.h"

ClientConnection::ClientConnection(boost::asio::io_service& ioService, ssl::context& context, ApoapseClient& client)
	: GenericConnection(ioService, context)
	, client(client)
{

}

ClientConnection::~ClientConnection()
{
	LOG_DEBUG << "~ClientConnection";
	client.OnDisconnect();
}

bool ClientConnection::OnReceivedError(const boost::system::error_code& error)
{
	LOG << error.message() << LogSeverity::warning;
	client.OnDisconnect();

	return true;
}

bool ClientConnection::IsAuthenticated() const
{
	return client.IsAuthenticated();
}

std::optional<Username> ClientConnection::GetConnectedUser() const
{
	if (IsAuthenticated())
		return client.GetLocalUser().username;
	else
		return std::optional<Username>();
}

bool ClientConnection::OnConnectedToServer()
{
	LOG_DEBUG << "OnConnectedToServer";
	client.OnConnectedToServer();

	return true;
}

void ClientConnection::OnReceivedCommand(CommandV2& cmd)
{
	const IUser* relatedUser = (IsAuthenticated() ? &client.GetLocalUser() : nullptr);

	if (cmd.IsValid(relatedUser))
		global->cmdManager->OnReceivedCmdInternal(cmd, *this);
	else
		ApoapseError(ApoapseErrors::invalid_cmd, this);
}
