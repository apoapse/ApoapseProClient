#include "stdafx.h"

#ifdef UNIT_TESTS

#include "Common.h"
#include "UnitTestsManager.h"
#include "UsergroupBlock.h"
#include "Random.hpp"
#include "UsergroupsManager.h"
#include "User.h"
#include "IUsersManager.h"

UNIT_TEST("UsergroupBlock:AddToCmd_and_validate")
{
	struct UsersManager : public IUsersManager
	{
		Username username1;
		Username author;
		PrivateKeyBytes authorPrivateKey;
		PublicKeyBytes authorPublicKey;
		PrivateKeyBytes username1PrivateKey;
		PublicKeyBytes username1PublicKey;

		UsersManager()
		{
			username1 = Username(Cryptography::GenerateRandomBytes(sha256Length));
			author = Username(Cryptography::GenerateRandomBytes(sha256Length));

			const auto[tauthorPrivateKey, tauthorPublicKey] = User::GenerateIdentityKey();
			authorPrivateKey = tauthorPrivateKey;
			authorPublicKey = tauthorPublicKey;

			const auto[tusername1PrivateKey, tusername1PublicKey] = User::GenerateIdentityKey();
			username1PrivateKey = tusername1PrivateKey;
			username1PublicKey = tusername1PublicKey;
		}

		bool DoesUserExist(const Username& username) const override
		{
			return (username == username1 || username == author);
		}

		PublicKeyBytes GetUserIdentityPublicKey(const Username& username) const override
		{
			return (username == username1) ? username1PublicKey : authorPublicKey;
		}
	};

	UsersManager localUsersManager;
	MessagePackSerializer ser;

	{
		UsergroupBlock block;
		block.version = 1;
		block.userList = std::vector<Username>{ localUsersManager.username1 };
		block.usergroupUuid = Uuid::Generate();
		block.permissions = UsergroupsManager::GetAllowedPermissions();

		block.AddBlockToCmd(ser, localUsersManager.author, localUsersManager.authorPrivateKey, localUsersManager);
	}

	MessagePackDeserializer deser(ser.GetMessagePackBytes());

	TEST_REQUIRE(UsergroupBlock::CreateFromCommand(deser, localUsersManager).has_value());
} UNIT_TEST_END

#endif	// UNIT_TESTS