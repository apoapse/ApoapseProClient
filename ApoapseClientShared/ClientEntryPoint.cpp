#include "stdafx.h"
#include "ClientEntryPoint.h"
#include "Common.h"
#include "LibraryLoader.hpp"

#ifdef UNIT_TESTS
#include "UnitTestsManager.h"
#endif //UNIT_TESTS

int ClientMain(const std::vector<std::string>& launchArgs)
{
	// Initialize global systems
	{
		ASSERT(global == nullptr);
		global = Global::CreateGlobal();

		global->logger = std::make_unique<Logger>("log_client.txt");
		global->threadPool = std::make_unique<ThreadPool>("Global thread pool", 8); // #TODO dynamically choose the number of threads into the global thread pool
	}

	// Run unit tests if requested
#ifdef UNIT_TESTS
	if (std::find(launchArgs.begin(), launchArgs.end(), "-run_unit_tests") != launchArgs.end())
	{
		LOG << "Starting Unit tests";
		UnitTestsManager::GetInstance().RunTests();
		return 0;
	}
#endif //UNIT_TESTS

	// Database
	boost::shared_ptr<IDatabase> databaseSharedPtr = LibraryLoader::LoadLibrary<IDatabase>("DatabaseImpl.sqlite");
	global->database = databaseSharedPtr.get();
	const char* dbParams[1];
	dbParams[0] = "server_database.db";
	if (databaseSharedPtr->Open(dbParams, 1))
	{
		LOG << "Database accessed successfully. Params: " << *dbParams;
	}
	else
	{
		FatalError("Unable to access the database");
	}

	{
// 		boost::asio::io_service ioService;
// 		auto connection = std::make_shared<GenericConnection>(ioService);
// 		connection->Connect("127.0.0.1", 5700);
// 
// 		auto test = std::vector<byte>{ 'a', 'b', 'c', 'd' };
// 		connection->Send(Commands::CONNECT, std::make_unique<std::vector<byte>>(test), nullptr);
// 		ioService.run();
	}

	return 1;
}