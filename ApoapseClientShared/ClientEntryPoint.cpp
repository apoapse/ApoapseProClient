#include "stdafx.h"
#include "ClientEntryPoint.h"
#include "Common.h"
#include "LibraryLoader.hpp"
#include "HTMLUI.h"

#ifdef UNIT_TESTS
#include "UnitTestsManager.h"
#endif //UNIT_TESTS

int ApoapseClient::ClientMain(const std::vector<std::string>& launchArgs)
{
	// Initialize global systems
	{
		ASSERT(global == nullptr);
		global = Global::CreateGlobal();

		global->logger = std::make_unique<Logger>("log_client.txt");
		global->threadPool = std::make_unique<ThreadPool>("Global thread pool", 8); // #TODO dynamically choose the number of threads into the global thread pool
		global->htmlUI = new HTMLUI;
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

	return 1;
}

void ApoapseClient::Shutdown()
{
	delete global->htmlUI;
}

std::vector<byte> ApoapseClient::ReadFile(const std::string& filename, const std::string& fileExtension)
{
	return global->htmlUI->GetWebResourcesManager().ReadFile(filename, fileExtension);
}

void ApoapseClient::RegisterSignalSender(ISignalSender* signalSender)
{
	global->htmlUI->RegisterSignalSender(signalSender);
}

std::string ApoapseClient::OnReceivedSignal(const std::string& name, const std::string& data)
{
	return global->htmlUI->OnReceivedSignal(name, data);
}
