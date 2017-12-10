#include "stdafx.h"
#include "ClientConnection.h"
#include "Common.h"
#include "ApoapseClient.h"
#include "HTMLUI.h"
#include "SecurityAlert.h"

ClientConnection::ClientConnection(boost::asio::io_service& ioService, ApoapseClient& client)
	: GenericConnection(ioService),
	client(client)
{

}

ClientConnection::~ClientConnection()
{
	LOG_DEBUG << "~ClientConnection";
	client.OnDisconnect(IsAuthenticated());
}

bool ClientConnection::OnReceivedError(const boost::system::error_code& error)
{
	LOG << error.message() << LogSeverity::warning;
	client.OnDisconnect(IsAuthenticated());

	return true;
}

bool ClientConnection::IsAuthenticated() const
{
	return m_isAuthenticated;
}

void ClientConnection::Authenticate()
{
	m_isAuthenticated = true;
}

bool ClientConnection::OnConnectedToServer()
{
	LOG_DEBUG << "OnConnectedToServer";
	client.OnConnectedToServer();

	return true;
}

void ClientConnection::OnReceivedValidCommand(std::unique_ptr<Command> cmd)
{
	const bool authenticated = IsAuthenticated();

	if (cmd->GetInfo().serverOnly)
	{
		SecurityLog::LogAlert(ApoapseErrorCode::cannot_processs_cmd_from_this_connection_type, *this);
		return;
	}

	try
	{
		if (cmd->GetInfo().requireAuthentication && authenticated)
		{
			cmd->Process(*this);
		}
		else if (cmd->GetInfo().onlyNonAuthenticated && !authenticated)
		{
			cmd->Process(*this);
		}
		else if (!cmd->GetInfo().requireAuthentication && !authenticated)
		{
			cmd->Process(*this);
		}
		else
		{
			SecurityLog::LogAlert(ApoapseErrorCode::cannot_processs_cmd_from_this_connection_type, *this);
		}
	}
	catch (const std::exception& e)
	{
		LOG << LogSeverity::error << "Exception trigged while processing a command of type " << static_cast<UInt16>(cmd->GetInfo().command) << ": " << e;
		Close();
	}
}