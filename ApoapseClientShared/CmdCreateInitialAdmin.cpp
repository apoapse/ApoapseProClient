#include "stdafx.h"
#include "CmdCreateInitialAdmin.h"
#include "Common.h"
#include "CommandsManager.h"
#include "ApoapseClient.h"
#include "ClientConnection.h"
#include "Uuid.h"
#include "User.h"

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
	const auto[idPrivateKey, idPublicKey] = User::GenerateIdentityKey();
	const auto [encryptedIdentityPrivateKey, iv] = User::EncryptIdentityPrivateKey(idPrivateKey, User::HashPasswordForIdentityPrivateKey(strPassword));

	MessagePackSerializer ser;
	ser.UnorderedAppend("usergroup.uuid", usergroupUuid.GetAsByteVector());
	ser.UnorderedAppend("user.username", username.GetRaw());
	ser.UnorderedAppend("user.password", hashedPassword);
	ser.UnorderedAppend("user.public_key", idPublicKey);
	ser.UnorderedAppend("user.private_key.encrypted", encryptedIdentityPrivateKey);
	ser.UnorderedAppend("user.private_key.iv", iv);


	/*
	ser.UnorderedAppend("usergroup_block.uuid", Uuid::Generate().GetAsByteVector());

	{
		const auto[privateKey, publicKey] = Cryptography::RSA::GenerateKeyPair(2048);
		const auto encryptedPrivateKey = Cryptography::SHA256(std::vector<byte>(privateKey.begin(), privateKey.end()));	//TEMP

		const auto users = std::vector<std::vector<byte>>{ std::vector<byte>(user1.begin(), user1.end()), std::vector<byte>(user2.begin(), user2.end()) };

		MessagePackSerializer serBlock;
		serBlock.UnorderedAppend("version", 2);
		serBlock.UnorderedAppend("date_time", DateTimeUtils::UTCDateTime::CurrentTime().str());	// Check if inferior or equal to the current date but also that is greater than the previous block date
		serBlock.UnorderedAppend("keypair_algorithm", "RSA2048"s);
		serBlock.UnorderedAppend("public_key", publicKey);
		serBlock.UnorderedAppend("private_key.algorithm", "AES256"s);
		serBlock.UnorderedAppend("private_key.encrypted", encryptedPrivateKey);
		serBlock.UnorderedAppendArray("users", users);
		serBlock.UnorderedAppendArray<std::string>("permissions", std::vector<std::string>{ "CREATE_USER", "CREATE_USERGROUP"});
		serBlock.UnorderedAppend("mac_algorithm", "RSA SHA256"s);
		serBlock.UnorderedAppend("mac_user_signer", user1);
		serBlock.UnorderedAppend("previous_block.hash_algorithm", "SHA3 256"s);
		serBlock.UnorderedAppend("previous_block.hash", Cryptography::SHA3_256(std::vector<byte> { 0xff, 0x00}));

		{
			std::vector<MessagePackSerializer> usersKey;

			for (size_t i = 0; i < 3; i++)
			{
				MessagePackSerializer serUser;
				serUser.UnorderedAppend("username", user1);
				serUser.UnorderedAppend("decryption_key", privateKeyTest);

				usersKey.push_back(serUser);
			}

			serBlock.UnorderedAppendArray("users_keys", usersKey);
		}

		blockMac = Cryptography::RSA::Sign(privateKey, serBlock.GetMessagePackBytes());
		ser.UnorderedAppend("usergroup_block.block", serBlock);
	}

	ser.UnorderedAppend("usergroup_block.mac", blockMac);
	*/

	CmdCreateInitialAdmin cmd;
	cmd.Send(ser, *client.GetConnection());
}

// Système d'enegistrement d'event dans Process() pour par exemple dire : appellez cette fonction lorsque la connecté est déconnectée ou appelez cette fonction lorsque une commande précise est reçue