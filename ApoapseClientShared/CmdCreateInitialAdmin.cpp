#include "stdafx.h"
#include "CmdCreateInitialAdmin.h"
#include "Common.h"
#include "CommandsManager.h"
#include "ApoapseClient.h"
#include "ClientConnection.h"
#include "Uuid.h"
#include "User.h"
#include "UsergroupBlock.h"
#include "UsergroupsManager.h"

CommandInfo& CmdCreateInitialAdmin::GetInfo() const
{
	static auto info = CommandInfo();
	info.command = CommandId::create_initial_admin;
	info.serverOnly = true;
	info.onlyNonAuthenticated = true;

	return info;
}

void CmdCreateInitialAdmin::CreateAndSend(const std::string& strUsername, const std::string& strPassword, ApoapseClient& client)
{
	const auto usergroupUuid = Uuid::Generate();
	const Username username = User::HashUsername(strUsername);
	const auto hashedPassword = User::HashPasswordForServer(strPassword);
	const auto [idPrivateKey, idPublicKey] = User::GenerateIdentityKey();
	const auto [encryptedIdentityPrivateKey, iv] = User::EncryptIdentityPrivateKey(idPrivateKey, User::HashPasswordForIdentityPrivateKey(strPassword));

	MessagePackSerializer ser;
	ser.UnorderedAppend("usergroup.uuid", usergroupUuid.GetAsByteVector());
	ser.UnorderedAppend("user.username", username.GetRaw());
	ser.UnorderedAppend("user.password", hashedPassword);
	ser.UnorderedAppend("user.public_key", idPublicKey);
	ser.UnorderedAppend("user.private_key_encrypted", encryptedIdentityPrivateKey);
	ser.UnorderedAppend("user.private_key_iv", iv);

	UsergroupBlock block;
	block.version = 1;
	block.userList = std::vector<Username>{ username };
	block.usergroupUuid = usergroupUuid;
	block.permissions = UsergroupsManager::GetAllowedPermissions();

	DummyUsersManager dummyUsersManager(username, idPublicKey);
	block.AddBlockToCmd(ser, username, idPrivateKey, dummyUsersManager);

	CmdCreateInitialAdmin cmd;
	cmd.Send(ser, *client.GetConnection());
}

// Système d'enegistrement d'event dans Process() pour par exemple dire : appellez cette fonction lorsque la connecté est déconnectée ou appelez cette fonction lorsque une commande précise est reçue
