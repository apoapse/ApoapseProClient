#pragma once
#include "Json.hpp"
#include <optional>
#include "CryptographyTypes.hpp"
#include "LocalUser.h"
#include <optional>
#include <thread>
#include <deque>
#include "Database.hpp"
#include "CommandV2.h"
#include "ContentManager.h"
#include "ClientOperations.h"
#include "User.h"
#include "UsergroupManager.h"
#include "Attachment.h"
#include "DatabaseSettings.h"
#include "JsonFileSettings.h"
class ClientConnection;
class ClientFileStreamConnection;
class ApoapseError;

class ApoapseClient
{
	std::thread m_ioServiceThread;
	std::thread m_fileStreamIoServiceThread;
	std::unique_ptr<boost::asio::io_service> m_fileStreamIOService;
	std::unique_ptr<boost::asio::io_service> m_mainConnectionIOService;
	
	std::shared_ptr<ClientConnection> m_connection;
	std::shared_ptr<ClientFileStreamConnection> m_fileStreamConnection;
	bool m_connected = false;
	
	std::optional<CommandV2> m_loginCmd;
	//hashSecBytes m_identityPasswordHash;
	Username m_lastLoginTryUsername;
	std::optional<std::string> m_dbPassword;
	std::optional<LocalUser> m_authenticatedUser;
	bool m_firstConnection = false;

	std::deque<Attachment::File> m_lastDroppedFiles;

	std::unique_ptr<UsergroupManager> m_usergroupManager;
	std::unique_ptr<ClientUsers> m_clientUsers;
	std::unique_ptr<ContentManager> m_contentManager;
	std::unique_ptr<ClientOperations> m_clientOperations;
	
public:
	DatabaseSettings serverSettings;
	JsonFileSettings clientSettings;
	
	ApoapseClient();
	//virtual ~ApoapseClient();

	//std::string OnReceivedSignal(const std::string& name, const std::string& data);
	std::string OnReceivedSignal(const std::string& name, const JsonHelper& deserializer);

	ClientConnection* GetConnection();
	ClientFileStreamConnection* GetFileStreamConnection() const;
	bool IsConnectedToServer() const;
	void Disconnect();

	void Connect(const std::string& serverAddress, const std::string& username, const std::string& password);
	void OnConnectedToServer();
	void OnDisconnect();
	const Username& GetLastLoginTryUsername() const;
	void OnReceivedError(ApoapseError& error);

	void AuthenticateFileStream(const std::vector<byte>& authCode);
	void Authenticate(const LocalUser& user);
	bool IsAuthenticated() const;
//	const hashSecBytes& GetIdentityPasswordHash() const;
	const LocalUser& GetLocalUser() const;

	void OnDragFiles(const std::vector<std::string> filesRaw);
	void OnDropFiles();
	void SendQueuedDroppedFile();
	std::vector<Attachment::File> GetDroppedFilesToSend();
	void OnFileUploaded();

	ContentManager& GetContentManager() const;
	ClientOperations& GetClientOperations() const;
	ClientUsers& GetClientUsers() const;
	static class ClientCmdManager* GetCmdManager();

	void InitUsergroupManager(std::vector<DataStructure>& usergroupsDat);
	UsergroupManager& GetUsergroupManager() const;
	void RefreshUserInfo() const;

	std::string GetAttachmentsDirectory();
	
private:
	static std::string GenerateDbPassword(const std::string& password);
	void OnAuthenticated();
	bool LoadDatabase();
	void UnloadDatabase();
	static std::tuple<std::string, UInt16> ParseAddress(const std::string& address);
};