#include "stdafx.h"
#include "ClientConnection.h"
#include "Common.h"
#include "ApoapseClient.h"
#include "HTMLUI.h"
#include "SecurityAlert.h"

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

bool ClientConnection::OnConnectedToServer()
{
	LOG_DEBUG << "OnConnectedToServer";
	client.OnConnectedToServer();

	return true;
}

void ClientConnection::OnReceivedValidCommand(CommandV2& cmd)
{
	global->cmdManager->OnReceivedCmdInternal(cmd, *this);
}
