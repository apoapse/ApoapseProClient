#pragma once
#include "Json.hpp"
#include <optional>
#include "CmdConnect.h"
#include "CryptographyTypes.hpp"
#include "LocalUser.h"
#include <optional>
#include <thread>
#include "RoomManager.h"
#include <boost/shared_ptr.hpp>
#include "Database.hpp"
class ClientConnection;

class ApoapseClient
{
	boost::shared_ptr<IDatabase> m_databaseSharedPtr;
	std::unique_ptr<boost::asio::io_service> m_IOService;
	std::thread m_ioServiceThread;
	ClientConnection* m_connection;
	bool m_connected;
	std::optional<std::unique_ptr<CmdConnect>> m_loginCmd;
	hashSecBytes m_identityPasswordHash;
	Username m_lastLoginTryUsername;
	std::optional<LocalUser> m_authenticatedUser;

	std::unique_ptr<RoomManager> m_roomManager;

public:
	ApoapseClient();
	//virtual ~ApoapseClient();

	//std::string OnReceivedSignal(const std::string& name, const std::string& data);
	std::string OnReceivedSignal(const std::string& name, const JsonHelper& deserializer);

	ClientConnection* GetConnection() const;
	bool IsConnectedToServer() const;

	void Connect(const std::string& serverAddress, const std::string& username, const std::string& password);
	void OnConnectedToServer();
	void OnSetupState();
	void OnUserFirstConnection();
	void OnDisconnect();
	const Username& GetLastLoginTryUsername() const;

	void Authenticate(const LocalUser& user);
	bool IsAuthenticated() const;
	const hashSecBytes& GetIdentityPasswordHash() const;
	const LocalUser& GetLocalUser() const;

	RoomManager& GetRoomManager() const;
private:

	void OnAuthenticated();
	bool LoadDatabase();
	void UnloadDatabase();
};