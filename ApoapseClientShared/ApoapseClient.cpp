#include "stdafx.h"
#include "ApoapseClient.h"
#include "Common.h"
#include "Json.hpp"
#include "ClientConnection.h"
#include "HTMLUI.h"
#include "User.h"
#include "GlobalVarDefines.hpp"
#include "LibraryLoader.hpp"
#include "DatabaseIntegrityPatcher.h"
#include "Hash.hpp"
#include "CommandsManagerV2.h"
#include "ClientFileStreamConnection.h"
#include "Attachment.h"
#include "ThreadUtils.h"
#include "NativeUI.h"
#include "ImageUtils.h"
#include "ApoapseError.h"
#include "FileUtils.h"

ApoapseClient::ApoapseClient()
{
	{
		const auto jsonData = FileUtils::ReadFile("client_global_settings.json");
		m_clientSettingsTxt = std::string(jsonData.begin(), jsonData.end());

		// We have to clean the json file first 
		m_clientSettingsTxt = std::regex_replace(m_clientSettingsTxt.value(), std::regex("\\n"), "");
		m_clientSettingsTxt = std::regex_replace(m_clientSettingsTxt.value(), std::regex("\\r"), "");
		m_clientSettingsTxt = std::regex_replace(m_clientSettingsTxt.value(), std::regex("\\t"), "");
		
		clientSettings = JsonHelper(m_clientSettingsTxt.value());
	}
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
		LOG << "Starting login hashing";
		//m_identityPasswordHash = User::HashPasswordForIdentityPrivateKey(password); // #MVP
		m_lastLoginTryUsername = User::HashUsername(username);
		m_dbPassword = GenerateDbPassword(password);

		DataStructure data = global->apoapseData->GetStructure("client_login");
		data.GetField("protocol_version").SetValue((Int64)protocolVersion);
		data.GetField("username").SetValue(m_lastLoginTryUsername);
		data.GetField("password").SetValue(User::HashPasswordForServer(password));

		m_loginCmd = global->cmdManager->CreateCommand("login", data);

		LOG << "Login hashing complete";
	}

	{
		const auto[address, mainPort] = ParseAddress(serverAddress);
		const UInt16 fileStreamPort = mainPort + 1;
		
		m_mainConnectionIOService = std::make_unique<boost::asio::io_service>();
		m_fileStreamIOService = std::make_unique<boost::asio::io_service>();

		global->htmlUI->UpdateStatusBar("@connecting_status");
		ssl::context tlsContext(ssl::context::sslv23);
		tlsContext.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 | ssl::context::no_tlsv1_1 | ssl::context::no_tlsv1 | ssl::context::single_dh_use | ssl::context::no_compression);
		tlsContext.set_default_verify_paths();

		// File Stream
		{
			m_fileStreamConnection = std::make_shared<ClientFileStreamConnection>(*m_fileStreamIOService, tlsContext, *this);
			m_fileStreamConnection->Connect(address, fileStreamPort);

			LOG << "[File stream connection] TCP Client started to " << serverAddress << " port: " << fileStreamPort;
		}
		
		// Main connection
		{
			m_connection = std::make_shared<ClientConnection>(*m_mainConnectionIOService, tlsContext, *this);
			m_connection->Connect(address, mainPort);

			LOG << "[Main connection] TCP Client started to " << serverAddress << " port: " << mainPort;
		}
	}

	// File Stream service
	m_fileStreamIoServiceThread = std::thread([this]
	{
		ThreadUtils::NameThread("File Stream thread");
		m_fileStreamIOService->run();
	});
	m_fileStreamIoServiceThread.detach();

	// Main connection service
	m_ioServiceThread = std::thread([this]
	{
		ThreadUtils::NameThread("Main connection thread");
		m_mainConnectionIOService->run();
	});
	m_ioServiceThread.detach();
}

ClientConnection* ApoapseClient::GetConnection()
{
	return m_connection.get();
}

ClientFileStreamConnection* ApoapseClient::GetFileStreamConnection() const
{
	return m_fileStreamConnection.get();
}

bool ApoapseClient::IsConnectedToServer() const
{
	return m_connected;
}

void ApoapseClient::Disconnect()
{
	m_connection->Close();
	m_fileStreamConnection->Close();

	m_connection.reset();
	m_fileStreamConnection.reset();
}

std::string ApoapseClient::OnReceivedSignal(const std::string& name, const JsonHelper& json)
{
	if (name.substr(0, 4) == "cmd_")
	{
		const std::string cmdName = name.substr(4, name.length());
		const auto cmdDef = global->cmdManager->GetCmdDefByFullName(cmdName);
		auto dataStructure = global->apoapseData->FromJSON(cmdDef.relatedDataStructure, json);

		global->cmdManager->CreateCommand(cmdName, dataStructure).Send(*m_connection);
	}

	else if (name == "OnUIReady")
	{
		global->htmlUI->SendSignal("SetClientGlobalSettings", m_clientSettingsTxt.value());
		m_clientSettingsTxt.reset();
	}

	else if (name == "OnFilesDropped")
	{
		OnDropFiles();
	}

	else if (name == "OpenFileDialog")
	{
		const std::string dialogName = json.ReadFieldValue<std::string>("name").value();
		
		global->threadPool->PushTask([dialogName]
		{
			const std::string res = NativeUI::OpenFileDialog(std::vector<std::string>{"*.jpg;*.jpeg;*.png;*.gif"}, "Images");

			if (!res.empty())
			{
				JsonHelper ser;
				ser.Insert("filepath", res);
				ser.Insert("name", dialogName);
				
				global->htmlUI->SendSignal("OnFileDialogPathSet", ser.Generate());
			}
		});
	}

	else if (name == "openURL")
	{
		const std::string cmd = "start " + json.ReadFieldValue<std::string>("url").value();
		std::system(cmd.c_str());
	}

	else if (name == "disconnect" && m_connected)
	{
		LOG << "User requested disconnection";
		Disconnect();
	}

	else if (name == "login" && !m_connected)
	{
		Connect(json.ReadFieldValue<std::string>("server").get(), json.ReadFieldValue<std::string>("username").get(), json.ReadFieldValue<std::string>("password").get());
	}

	else if (name == "user_first_connection" && m_connected)
	{
		const ByteContainer password = User::HashPasswordForServer(json.ReadFieldValue<std::string>("password").get());

		auto dat = global->apoapseData->GetStructure("set_identity");
		dat.GetField("password").SetValue(password);
		dat.GetField("nickname").SetValue(json.ReadFieldValue<std::string>("nickname").get());
		
		const std::string avatarPath = json.ReadFieldValue<std::string>("avatar_path").value();
		if (!avatarPath.empty())
		{
			const auto imgBytes = ImageUtils::ReadAndResizeImage(avatarPath, 128, 128, true, "jpg");
			dat.GetField("avatar").SetValue(imgBytes);
		}

		global->cmdManager->CreateCommand("set_identity", dat).Send(*m_connection);
	}

	else if (name == "request_random_password" && m_connected)
	{
		JsonHelper ser;
		ser.Insert("randomPassword", User::GenerateTemporaryRandomPassword());

		return ser.Generate();
	}

	else if (name == "register_user" && m_connected)
	{
		const auto tempPassword = User::GenerateTemporaryRandomPassword();
		const std::string username = json.ReadFieldValue<std::string>("username").get();
		const std::string usergroup = json.ReadFieldValue<std::string>("usergroup").get();

		{
			JsonHelper ser;
			ser.Insert("username", username);
			ser.Insert("temp_password", tempPassword);

			global->htmlUI->SendSignal("OnAddNewUserLocal", ser.Generate());
		}

		auto dat = global->apoapseData->GetStructure("add_user");
		dat.GetField("username").SetValue(User::HashUsername(username));
		dat.GetField("temp_password").SetValue(User::HashPasswordForServer(tempPassword));
		dat.GetField("usergroup").SetValue(GetUsergroupManager().GetUsergroup(usergroup).GetUuid());

		global->cmdManager->CreateCommand("add_user", dat).Send(*m_connection);
	}

	else if (name == "AddTag")
	{
		const std::string itemType = json.ReadFieldValue<std::string>("item_type").get();

		auto dat = global->apoapseData->GetStructure("tag");
		dat.GetField("name").SetValue(json.ReadFieldValue<std::string>("name").get());
		dat.GetField("item_type").SetValue(itemType);

		if (itemType == "msg")
		{
			auto& relatedMsg = GetContentManager().GetCurrentThread().GetMessageById(json.ReadFieldValue<Int64>("msg_id").get());
			dat.GetField("item_uuid").SetValue(relatedMsg.uuid);
		}

		global->cmdManager->CreateCommand("add_tag", dat).Send(*m_connection);
	}

	else if (name == "mark_message_as_read")
	{
		const DbId msgId = json.ReadFieldValue<Int64>("id").get();
		
		Uuid itemUuid;
		if (GetContentManager().IsThreadDisplayed())
		{
			const DbId threadId = json.ReadFieldValue<Int64>("threadId").get();
			itemUuid = GetContentManager().GetThreadById(threadId).GetMessageById(msgId).uuid;
		}
		else if (GetContentManager().IsUserPageDisplayed())
		{
			itemUuid = GetContentManager().GetCurrentUserPage().GetMessageById(msgId).uuid;
		}

		auto dat = global->apoapseData->GetStructure("mark_as_read");
		dat.GetField("item_type").SetValue("msg");
		dat.GetField("item_uuid").SetValue(itemUuid);

		global->cmdManager->CreateCommand("mark_as_read", dat).Send(*m_connection);
	}
	else if (name == "openAttachment")
	{
		std::shared_ptr<Attachment> att = GetContentManager().GetAttachment((DbId)json.ReadFieldValue<Int64>("id").value());
		att->RequestOpenFile();
	}

	else
	{
		if (IsAuthenticated())
			m_contentManager->OnReceivedSignal(name, json);
	}

	return "";
}

void ApoapseClient::OnConnectedToServer()
{
	m_connected = true;
	global->htmlUI->UpdateStatusBar("@connected_waiting_authentication");

	m_loginCmd->Send(*m_connection);
	m_loginCmd.reset();
}

void ApoapseClient::OnDisconnect()
{
	m_connected = false;
	m_connection = nullptr;
	m_authenticatedUser.reset();
	m_loginCmd.reset();

	m_usergroupManager.reset();
	m_clientUsers.reset();
	m_contentManager.reset();
	m_clientOperations.reset();

	if (global->database != nullptr)
		UnloadDatabase();

	global->htmlUI->UpdateStatusBar("@disconnected_status", true);

	global->htmlUI->SendSignal("OnDisconnect", "");
}

const Username& ApoapseClient::GetLastLoginTryUsername() const
{
	return m_lastLoginTryUsername;
}

void ApoapseClient::OnReceivedError(ApoapseError& error)
{
	// Display on UI
	{
		JsonHelper ser;
		ser.Insert("name", error.GetErrorStr());

		global->htmlUI->SendSignal("OnServerError", ser.Generate());
	}

	// Disconnect
	if (m_fileStreamConnection)
		m_fileStreamConnection->Close();

	if (m_connection && m_connection->IsConnected())
		m_connection->Close();
}

void ApoapseClient::AuthenticateFileStream(const std::vector<byte>& authCode)
{
	auto dat = std::make_shared<ByteContainer>();
	dat->reserve(sha256Length + sha256Length);
	
	auto& username = GetLocalUser().username;
	std::copy(username.GetRaw().begin(), username.GetRaw().end(), std::back_inserter(*dat));
	std::copy(authCode.begin(), authCode.end(), std::back_inserter(*dat));
	
	m_fileStreamConnection->Send(dat);
}

void ApoapseClient::Authenticate(const LocalUser& user)
{
	m_authenticatedUser = user;

	LOG << "User " << m_authenticatedUser->nickname << " of usergroup " << m_authenticatedUser->usergroup->GetName() << " authenticated successfuly";

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
			Disconnect();
			return;
		}

		DatabaseIntegrityPatcher dbIntegrity;
		if (!dbIntegrity.CheckAndResolve())
		{
			LOG << LogSeverity::error << "The database integrity patcher has failed";
			Disconnect();
			return;
		}
	}
	
	// Systems
	m_clientUsers = std::make_unique<ClientUsers>(*this);
	m_clientOperations = std::make_unique<ClientOperations>();

	m_contentManager = std::make_unique<ContentManager>(*this);
	m_contentManager->Init();

	// UI
	/*global->htmlUI->UpdateStatusBar("@connected_and_authenticated_status", false);
	{
		JsonHelper json;
		json.Insert("localUser.username", m_authenticatedUser->username.ToStr());
		json.Insert("localUser.nickname", m_authenticatedUser->nickname);

		global->htmlUI->SendSignal("connected_and_authenticated", json.Generate());
	}*/

	// Apoapse sync
	m_clientOperations->SendSyncRequest(*m_connection);

	RefreshUserInfo();
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

	const std::string dbFileName = NativeUI::GetUserDirectory() + "user_" + m_authenticatedUser->username.ToStr() + ".db";
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

void ApoapseClient::RefreshUserInfo() const
{
	JsonHelper ser;
	ser.Insert("local_user", GetLocalUser().GetJson());

	for (const auto& usergroup : GetUsergroupManager().GetUsergroups())
	{
		JsonHelper userGrpSer;
		userGrpSer.Insert("name", usergroup.GetName());
		userGrpSer.InsertArray("usergroups", usergroup.GetPermissions());

		ser.Insert("usergroups", userGrpSer);
	}

	global->htmlUI->SendSignal("UpdateUserInfo", ser.Generate());
}

std::tuple<std::string, UInt16> ApoapseClient::ParseAddress(const std::string& address)
{
	if (StringExtensions::contains(address, ':'))
	{
		std::vector<std::string> spliced;
		StringExtensions::split(address, spliced, ":");

		return std::make_tuple(spliced.at(0), std::atoi(spliced.at(1).c_str()));
	}
	else
	{
		return std::make_tuple(address, defaultServerPort);
	}
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

void ApoapseClient::OnDradFiles(const std::vector<std::string> filesRaw)
{
	for (const std::string& filePath : filesRaw)
	{
		Attachment::File file = Attachment::File(filePath);
		file.uuid = Uuid::Generate();
		
		LOG << "Drag and Drop: user dragged file " << file.fileName << " size: " << file.fileSize;
		
		m_lastDroppedFiles.push_back(file);
	}
}

void ApoapseClient::OnDropFiles()
{
	if (!IsAuthenticated() || m_lastDroppedFiles.empty())
		return;

	LOG << "Dropped " << m_lastDroppedFiles.size() << " files";

	if (GetContentManager().IsThreadDisplayed() || GetContentManager().IsUserPageDisplayed())
	{
		JsonHelper ser;

		size_t i = 0;
		for (const auto& file : m_lastDroppedFiles)
		{
			JsonHelper attSer;
			attSer.Insert("fileName", HTMLUI::HtmlSpecialChars(file.fileName));
			attSer.Insert("fileSize", file.fileSize / 1000);	//Size in kb
			attSer.Insert("id", i);

			ser.Insert("attachments", attSer);
			i++;
		}
		
		global->htmlUI->SendSignal("OnDroppedFiles", ser.Generate());
	}
}

void ApoapseClient::SendQueuedDroppedFile()
{
	const Attachment::File att = m_lastDroppedFiles.front();
	const std::shared_ptr<Attachment> attachment = GetContentManager().GetAttachment(att.uuid);

	AttachmentFile file;
	file.uuid = att.uuid;
	file.fileName = att.fileName;
	file.fileSize = att.fileSize;
	file.filePath = att.filePath;
	
	GetFileStreamConnection()->PushFileToSend(file);
	
	{
		JsonHelper ser;
		ser.Insert("id", attachment->id);
		ser.Insert("status", "uploading");

		global->htmlUI->SendSignal("ChangeAttachmentStatus", ser.Generate());
	}

	// Save the file locally
	attachment->CopyFileLocally(att.filePath);
}

std::vector<Attachment::File> ApoapseClient::GetDroppedFilesToSend()
{
	std::vector<Attachment::File> out;

	for (auto& file : m_lastDroppedFiles)
	{
		if (!file.attached)
		{
			file.attached = true;
			out.push_back(file);
		}
	}

	return out;
}

void ApoapseClient::OnFileUploaded()
{
	ASSERT(!m_lastDroppedFiles.empty());

	const Uuid uuid = m_lastDroppedFiles.front().uuid;
	const std::shared_ptr<Attachment> attachment = GetContentManager().GetAttachment(uuid);
	
	JsonHelper ser;
	ser.Insert("id", attachment->id);
	ser.Insert("status", "ready");

	global->htmlUI->SendSignal("ChangeAttachmentStatus", ser.Generate());
	
	m_lastDroppedFiles.pop_front();

	if (!m_lastDroppedFiles.empty())
	{
		SendQueuedDroppedFile();
	}
}

ContentManager& ApoapseClient::GetContentManager() const
{
	ASSERT(IsAuthenticated());

	return *m_contentManager;
}

ClientOperations& ApoapseClient::GetClientOperations() const
{
	return *m_clientOperations;
}

ClientUsers& ApoapseClient::GetClientUsers() const
{
	return *m_clientUsers;
}

void ApoapseClient::InitUsergroupManager(std::vector<DataStructure>& usergroupsDat)
{
	m_usergroupManager = std::make_unique<UsergroupManager>(usergroupsDat);
}

UsergroupManager& ApoapseClient::GetUsergroupManager() const
{
	return *m_usergroupManager;
}
