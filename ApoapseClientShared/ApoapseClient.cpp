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
#include "CmdApoapseInstall.h"
#include "CmdFirstUserConnection.h"
#include "CmdSyncRequest.h"
#include "Operation.h"
#include "CmdMarkMessageAsRead.h"
#include "Hash.hpp"

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

	global->htmlUI->UpdateStatusBar("@password_encryption_status");
	{
		m_loginCmd = std::make_unique<CmdConnect>();

		LOG << "Starting login hashing";
		//m_identityPasswordHash = User::HashPasswordForIdentityPrivateKey(password); // #MVP
		m_lastLoginTryUsername = User::HashUsername(username);
		m_dbPassword = GenerateDbPassword(password);
		m_loginCmd.value()->PrepareLoginCmd(m_lastLoginTryUsername, password);

		LOG << "Login hashing complete";
	}

	{
		m_IOService = std::make_unique<boost::asio::io_service>();

		global->htmlUI->UpdateStatusBar("@connecting_status");
		const UInt16 port = defaultServerPort;
		ssl::context tlsContext(ssl::context::sslv23);

		auto connection = std::make_shared<ClientConnection>(*m_IOService, tlsContext, *this);
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
	if (name == "disconnect" && m_connected)
	{
		LOG << "User requested disconnection";
		m_connection->Close();
	}

	if (name == "login" && !m_connected)
	{
		Connect(json.ReadFieldValue<std::string>("server").get(), json.ReadFieldValue<std::string>("username").get(), json.ReadFieldValue<std::string>("password").get());
	}

	else if (name == "apoapse_install" && m_connected)
	{
		const auto username = User::HashUsername(json.ReadFieldValue<std::string>("admin_username").get());
		const auto password = User::HashPasswordForServer(json.ReadFieldValue<std::string>("admin_password").get());

		CmdApoapseInstall::SendInstallCommand(username, password, json.ReadFieldValue<std::string>("admin_nickname").get(), *this);
	}

	else if (name == "user_first_connection" && m_connected)
	{
		const auto password = User::HashPasswordForServer(json.ReadFieldValue<std::string>("password").get());
		CmdFirstUserConnection::SetUserIdentity(password, json.ReadFieldValue<std::string>("nickname").get(), *this);
	}

	else if (name == "request_random_password" && m_connected)
	{
		JsonHelper ser;
		ser.Insert("randomPassword", User::GenerateTemporaryRandomPassword());

		return ser.Generate();
	}

	else if (name == "register_user" && m_connected)
	{
		// #MVP Add permissions checks
		const auto tempPassword = User::GenerateTemporaryRandomPassword();
		const auto username = json.ReadFieldValue<std::string>("username").get();

		{
			JsonHelper json;
			json.Insert("username", username);
			json.Insert("temp_password", tempPassword);

			global->htmlUI->SendSignal("OnAddedNewUser", json.Generate());
		}

		const auto password = User::HashPasswordForServer(tempPassword);
		CmdRegisterNewUser::SendRegisterCommand(User::HashUsername(username), password, *this);
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

	else if (name == "mark_message_as_read" && m_connected && IsAuthenticated())
	{
		auto* activeThread = m_roomManager->GetActiveThread();
		ASSERT(activeThread != nullptr);

		auto* message = activeThread->GetMessageByDbid(json.ReadFieldValue<Int64>("dbid").get());
		ASSERT(message != nullptr);

		CmdMarkMessageAsRead::SendMarkMessageAsRead(message->uuid, *this);
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
	{
		JsonHelper ser;
		ser.Insert("previousUsername", GetLastLoginTryUsername());

		global->htmlUI->SendSignal("show_install", ser.Generate());
	}

	global->htmlUI->UpdateStatusBar("@connected_in_setup_phase_status");
}

void ApoapseClient::OnUserFirstConnection()
{
	global->htmlUI->SendSignal("ShowFirstUserConnection", "");
	global->htmlUI->UpdateStatusBar("@connected_first_user_connection_status");
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

	global->htmlUI->SendSignal("OnDisconnect", "");
}

const Username& ApoapseClient::GetLastLoginTryUsername() const
{
	return m_lastLoginTryUsername;
}

void ApoapseClient::Authenticate(const LocalUser& user)
{
	m_authenticatedUser = user;

	LOG << "User " << m_authenticatedUser->username.ToStr() << " authenticated.";

	OnAuthenticated();
}

std::string ApoapseClient::GenerateDbPassword(const std::string& password)
{
	std::vector<byte> output;
	auto digest = Cryptography::PBKDF2_SHA256(std::vector<byte>(password.begin(), password.end()), std::vector<byte>(), 3000);
	output.insert(output.begin(), digest.begin(), digest.end());

	return BytesToHexString(output);
}

void ApoapseClient::OnAuthenticated()
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
	{
		JsonHelper json;
		json.Insert("localUser.username", m_authenticatedUser->username.ToStr());
		json.Insert("localUser.nickname", m_authenticatedUser->nickname);

		global->htmlUI->SendSignal("connected_and_authenticated", json.Generate());
	}

	// Apoapse sync
	CmdSyncRequest::SendSyncRequest(Operation::GetMostRecentOperationTime(GetLocalUser().username), *this);
}

bool ApoapseClient::LoadDatabase()
{
#ifdef DO_NOT_ENCRYPT_DATABASE
	m_databaseSharedPtr = LibraryLoader::LoadLibrary<IDatabase>("DatabaseImpl.sqlite");
	LOG << "WARNING: THE DATABASE ENCRYPTION IS DISABLED" << LogSeverity::security_alert;
#else
	m_databaseSharedPtr = LibraryLoader::LoadLibrary<IDatabase>("DatabaseImpl.sqlcipher");
#endif

	global->database = m_databaseSharedPtr.get();

	std::vector<const char*> dbParams;

	std::string dbFileName = "user_" + m_authenticatedUser->username.ToStr() + ".db";
	dbParams.push_back(dbFileName.c_str());

#ifndef DO_NOT_ENCRYPT_DATABASE
	const std::string dbPassword = m_dbPassword.value();
	dbParams.push_back(dbPassword.c_str());

	m_dbPassword.reset();
#endif

	if (m_databaseSharedPtr->Open(dbParams.data(), dbParams.size()))
	{
		LOG << "Database accessed successfully.";
		return true;
	}
	else
	{
		LOG << "Unable to access the database." << LogSeverity::error;
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

/*const hashSecBytes& ApoapseClient::GetIdentityPasswordHash() const
{
	ASSERT(!m_identityPasswordHash.empty());
	return m_identityPasswordHash;
}*/

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
