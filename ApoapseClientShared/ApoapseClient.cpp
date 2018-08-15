#include "stdafx.h"
#include "ApoapseClient.h"
#include "Common.h"
#include "Json.hpp"
#include "ClientConnection.h"
#include "HTMLUI.h"
#include "User.h"
#include "GlobalVarDefines.hpp"
#include "CmdRegisterNewUser.h"
#include "LibraryLoader.hpp"
#include "DatabaseIntegrityPatcher.h"
#include "ClientDatabaseScheme.hpp"

ApoapseClient::ApoapseClient()
{

}

void ApoapseClient::Connect(const std::string& serverAddress, const std::string& username, const std::string& password)
{
	if (serverAddress.length() < 3)
	{
		global->htmlUI->UpdateStatusBar("@invalid_server_address", true);
		return;
	}
	/*else if (username.length() < 6 || password.length() < 8)// #TODO use values from the create user cmd
	{
	global->htmlUI->UpdateStatusBar("@invalid_login_input", true);
	return;
	}*/

	global->htmlUI->UpdateStatusBar("@password_encryption_status");
	{
		m_loginCmd = std::make_unique<CmdConnect>();

		LOG << "Starting login hashing";
		//m_identityPasswordHash = User::HashPasswordForIdentityPrivateKey(password); // #MVP
		m_lastLoginTryUsername = User::HashUsername(username);
		m_loginCmd.value()->PrepareLoginCmd(m_lastLoginTryUsername, password);

		LOG << "Login hashing complete";
	}

	{
		m_IOService = std::make_unique<boost::asio::io_service>();

		global->htmlUI->UpdateStatusBar("@connecting_status");
		const UInt16 port = defaultServerPort;
		auto connection = std::make_shared<ClientConnection>(*m_IOService, *this);
		connection->Connect(serverAddress, port);

		m_connection = connection.get();
		LOG << "TCP Client started to " << serverAddress << " port: " << port;
	}

	m_ioServiceThread = std::thread([this]
	{
		m_IOService->run();
	});
	m_ioServiceThread.detach();
}

ClientConnection* ApoapseClient::GetConnection() const
{
	return m_connection;
}

bool ApoapseClient::IsConnectedToServer() const
{
	return m_connected;
}

std::string ApoapseClient::OnReceivedSignal(const std::string& name, const JsonHelper& json)
{
	if (name == "login" && !m_connected)
	{
		Connect(json.ReadFieldValue<std::string>("server").get(), json.ReadFieldValue<std::string>("username").get(), json.ReadFieldValue<std::string>("password").get());
	}

	/*else if (name == "create_admin")
	{
		if (m_connected && !IsAuthenticated())
		{
			// #MVP Temporary, need to use a specific create_admin command in the future?

		}
		else
		{
			LOG << LogSeverity::error << "Trying to create an admin account but the the connection is not on setup state";
		}
	}*/

	else if (name == "register_user" && m_connected)
	{
		// #MVP Add permissions checks
		global->htmlUI->UpdateStatusBar("@password_encryption_status");

		const auto username = User::HashUsername(json.ReadFieldValue<std::string>("username").get());
		const auto password = User::HashPasswordForServer(json.ReadFieldValue<std::string>("password").get());
		CmdRegisterNewUser::SendRegisterCommand(username, password, *this);
		m_connection->Close();
	}

	else if (name == "create_new_room" && m_connected && IsAuthenticated())
	{
		// #MVP Add permissions checks
		m_roomManager->SendCreateNewRoom(json.ReadFieldValue<std::string>("name").get());
	}

	else if (name == "create_new_thread" && m_connected && IsAuthenticated())
	{
		m_roomManager->SendAddNewThread(json.ReadFieldValue<std::string>("name").get());
	}

	else if (name == "loadRoomUI" && m_connected && IsAuthenticated())
	{
		m_roomManager->SetUISelectedRoom(json.ReadFieldValue<Int64>("internalId").get());
	}

	else if (name == "loadThread" && m_connected && IsAuthenticated())
	{
		m_roomManager->SetActiveThread(json.ReadFieldValue<Int64>("internalId").get());
	}

	else if (name == "send_new_message" && m_connected && IsAuthenticated())
	{
		auto* activeThread = m_roomManager->GetActiveThread();
		ASSERT(activeThread != nullptr);

		activeThread->SendNewMessage(json.ReadFieldValue<std::string>("msg_content").get());
	}

	return "";
}

void ApoapseClient::OnConnectedToServer()
{
	m_connected = true;
	global->htmlUI->UpdateStatusBar("@connected_waiting_authentication");

	m_loginCmd.value()->Send(*m_connection);
	m_loginCmd.reset();
}

void ApoapseClient::OnSetupState()
{
	global->htmlUI->SendSignal("show_setup_state", "test");
	global->htmlUI->UpdateStatusBar("@connected_in_setup_phase_status");
}

void ApoapseClient::OnDisconnect()
{
	m_connected = false;
	m_connection = nullptr;
	m_authenticatedUser.reset();
	m_loginCmd.reset();

	m_IOService->reset();

	if (global->database != nullptr)
		UnloadDatabase();

	global->htmlUI->UpdateStatusBar("@disconnected_status", true);

	global->htmlUI->SendSignal("login_form_enable_back", "");
}

const Username& ApoapseClient::GetLastLoginTryUsername() const
{
	return m_lastLoginTryUsername;
}

void ApoapseClient::Authenticate(const LocalUser& localUser)
{
	m_authenticatedUser = localUser;
	LOG << "User " << localUser.username.ToStr() << " authenticated.";

	OnAuthenticated(localUser);
}

void ApoapseClient::OnAuthenticated(const LocalUser& localUser)
{
	// Database
	{
		if (!LoadDatabase())
		{
			m_connection->Close();
			return;
		}

		DatabaseIntegrityPatcher dbIntegrity(GetClientDbScheme());
		if (!dbIntegrity.CheckAndResolve())
		{
			LOG << LogSeverity::error << "The database integrity patcher has failed";
			m_connection->Close();
			return;
		}
	}

	// Systems
	m_roomManager = std::make_unique<RoomManager>(*this);
	m_roomManager->Initialize();

	// UI
	global->htmlUI->UpdateStatusBar("@connected_and_authenticated_status", false);
	global->htmlUI->SendSignal("connected_and_authenticated", ""s);
}

bool ApoapseClient::LoadDatabase()
{
	m_databaseSharedPtr = LibraryLoader::LoadLibrary<IDatabase>("DatabaseImpl.sqlite");
	global->database = m_databaseSharedPtr.get();
	const char* dbParams[1];
	std::string dbFileName = "user_" + GetLocalUser().username.ToStr() + ".db";
	dbParams[0] = dbFileName.c_str();
	if (m_databaseSharedPtr->Open(dbParams, 1))
	{
		LOG << "Database accessed successfully. Params: " << *dbParams;
		return true;
	}
	else
	{
		LOG << "Unable to access the database. Params: " << *dbParams << LogSeverity::error;
		return false;
	}
}

void ApoapseClient::UnloadDatabase()
{
	m_databaseSharedPtr->Close();
	m_databaseSharedPtr.reset();
	global->database = nullptr;
}

bool ApoapseClient::IsAuthenticated() const
{
	return m_authenticatedUser.has_value();
}

const hashSecBytes& ApoapseClient::GetIdentityPasswordHash() const
{
	ASSERT(!m_identityPasswordHash.empty());
	return m_identityPasswordHash;
}

const LocalUser& ApoapseClient::GetLocalUser() const
{
	ASSERT_MSG(IsAuthenticated(), "Trying to get the local user but the client is not authenticated");

	return m_authenticatedUser.value();
}

RoomManager& ApoapseClient::GetRoomManager() const
{
	ASSERT(IsAuthenticated());

	return *m_roomManager;
}
