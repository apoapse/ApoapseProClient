#pragma once
#include "Json.hpp"
#include <optional>
#include "CryptographyTypes.hpp"
#include "LocalUser.h"
#include <optional>
#include <thread>
#include <boost/shared_ptr.hpp>
#include "Database.hpp"
#include "CommandV2.h"
#include "ContentManager.h"
#include "ClientOperations.h"
#include "User.h"
#include "UsergroupManager.h"
class ClientConnection;

class ApoapseClient
{
	boost::shared_ptr<IDatabase> m_databaseSharedPtr;
	std::unique_ptr<boost::asio::io_service> m_IOService;
	std::thread m_ioServiceThread;
	ClientConnection* m_connection;
	bool m_connected;
	std::optional<CommandV2> m_loginCmd;
	//hashSecBytes m_identityPasswordHash;
	Username m_lastLoginTryUsername;
	std::optional<std::string> m_dbPassword;
	std::optional<LocalUser> m_authenticatedUser;

	std::vector<std::string> m_lastDroppedFiles;

	std::unique_ptr<UsergroupManager> m_usergroupManager;
	std::unique_ptr<ClientUsers> m_clientUsers;
	std::unique_ptr<ContentManager> m_contentManager;
	std::unique_ptr<ClientOperations> m_clientOperations;

public:
	ApoapseClient();
	//virtual ~ApoapseClient();

	//std::string OnReceivedSignal(const std::string& name, const std::string& data);
	std::string OnReceivedSignal(const std::string& name, const JsonHelper& deserializer);

	ClientConnection* GetConnection() const;
	bool IsConnectedToServer() const;

	void Connect(const std::string& serverAddress, const std::string& username, const std::string& password);
	void OnConnectedToServer();
	void OnDisconnect();
	const Username& GetLastLoginTryUsername() const;

	void Authenticate(const LocalUser& user);
	bool IsAuthenticated() const;
//	const hashSecBytes& GetIdentityPasswordHash() const;
	const LocalUser& GetLocalUser() const;

	void OnDropFiles(const std::vector<std::string> files);

	ContentManager& GetContentManager() const;
	ClientOperations& GetClientOperations() const;
	ClientUsers& GetClientUsers() const;

	void InitUsergroupManager(std::vector<DataStructure>& usergroupsDat);
	UsergroupManager& GetUsergroupManager() const;
private:
	static std::string GenerateDbPassword(const std::string& password);
	void OnAuthenticated();
	bool LoadDatabase();
	void UnloadDatabase();
	void RefreshUserInfo() const;
};