#pragma once
#include "Json.hpp"
#include <optional>
#include "CmdConnect.h"
#include "CryptographyTypes.hpp"
#include "LocalUser.h"
#include <optional>
class ClientConnection;

class ApoapseClient
{
	boost::asio::io_service m_IOService;
	ClientConnection* m_connection;
	bool m_connected;
	std::optional<std::unique_ptr<CmdConnect>> m_loginCmd;
	hashSecBytes m_identityPasswordHash;
	Username m_lastLoginTryUsername;
	std::optional<LocalUser> m_authenticatedUser;

public:
	ApoapseClient();
	//virtual ~ApoapseClient();

	std::string OnReceivedSignal(const std::string& name, const std::string& data);
	std::string OnReceivedSignal(const std::string& name, const JsonHelper& deserializer);

	ClientConnection* GetConnection() const;
	bool IsConnectedToServer() const;

	void Connect(const std::string& serverAddress, const std::string& username, const std::string& password);
	void OnConnectedToServer();
	void OnSetupState();
	void OnDisconnect();
	const Username& GetLastLoginTryUsername() const;

	void Authenticate(const LocalUser& localUser);
	bool IsAuthenticated() const;
	const hashSecBytes& GetIdentityPasswordHash() const;
	const LocalUser& GetLocalUser() const;
private:
};