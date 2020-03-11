// ----------------------------------------------------------------------------
// Copyright (C) 2020 Apoapse
// Copyright (C) 2020 Guillaume Puyal
//
// Distributed under the Apoapse Pro Client Software License. Non-commercial use only.
// See accompanying file LICENSE.md
//
// For more information visit https://github.com/apoapse/
// And https://apoapse.space/
// ----------------------------------------------------------------------------

#include "stdafx.h"
#include "ClientEntryPoint.h"
#include "Common.h"
#include "HTMLUI.h"
#include "ApoapseClient.h"
#include "DataStructures.hpp"
#include "ClientCmdManager.h"
#include "ThreadUtils.h"
#include <filesystem>
#include "NativeUI.h"
#include "LibraryLoader.hpp"

#ifdef UNIT_TESTS
#include "UnitTest.hpp"
#endif //UNIT_TESTS

ApoapseClient* m_apoapseClient = nullptr;
boost::shared_ptr<IDatabase> m_database;

int ApoapseClientEntry::ClientMain(const std::vector<std::string>& launchArgs)
{
	// Initialize global systems
	ASSERT(global == nullptr);
	global = Global::CreateGlobal();
	global->isClient = true;

	// Create the user folder if it does not exist
	if (!std::filesystem::exists(NativeUI::GetUserDirectory()))
	{
		std::filesystem::create_directory(NativeUI::GetUserDirectory());
	}
	
	global->apoapseData = std::make_unique<ApoapseData>(GetDataStructures());

	global->logger = std::make_unique<Logger>("log_client.txt");
	global->threadPool = std::make_unique<ThreadPool>("Global thread pool", 2, true);
	
#ifdef DO_NOT_ENCRYPT_DATABASE
	m_database = LibraryLoader::LoadLibrary<IDatabase>("DatabaseImpl.sqlite");
	LOG << "WARNING: THE DATABASE ENCRYPTION IS DISABLED" << LogSeverity::security_alert;
#else
	m_database = LibraryLoader::LoadLibrary<IDatabase>("DatabaseImpl.sqlcipher");
#endif

	global->database = m_database.get();
	global->database->Initialize();

	// Main thread
	{
		global->mainThread = std::make_unique<ThreadPool>("Main thread", 1, false);

		std::thread thread([]()
		{
			ThreadUtils::NameThread("Main Apoapse Thread");
			global->mainThread->Run();
		});
		thread.detach();
	}

	// Apoapse
	auto res = global->mainThread->PushTaskFuture([]()
	{
		m_apoapseClient = new ApoapseClient;
		global->cmdManager = std::make_unique<ClientCmdManager>(*m_apoapseClient);

		global->htmlUI = new HTMLUI(*m_apoapseClient);

		return true;
	});
	res.get();	// This is to make sure ApoapseClient and HTMLUI are initialized before the UI start to use them

	// Run unit tests if requested
#ifdef UNIT_TESTS
	if (std::find(launchArgs.begin(), launchArgs.end(), "-run_unit_tests") != launchArgs.end())
	{
		LOG << "Starting Unit tests";
		UnitTestsManager::GetInstance().RunTests(std::cout);
		return 0;
	}
#endif //UNIT_TESTS

	return 1;
}

void ApoapseClientEntry::Shutdown()
{
	delete global->htmlUI;
	delete m_apoapseClient;
}

std::vector<byte> ApoapseClientEntry::ReadFile(const std::string& filename, const std::string& fileExtension)
{
	return static_cast<HTMLUI*>(global->htmlUI)->GetWebResourcesManager().ReadFile(filename, fileExtension);
}

void ApoapseClientEntry::RegisterSignalSender(ISignalSender* signalSender)
{
	dynamic_cast<HTMLUI*>(global->htmlUI)->RegisterSignalSender(signalSender);
}

std::string ApoapseClientEntry::OnReceivedSignal(const std::string& name, const std::string& data)
{
	return dynamic_cast<HTMLUI*>(global->htmlUI)->OnReceivedSignal(name, data);
}

void ApoapseClientEntry::SetLastFilesDrop(const std::vector<std::string>& files)
{
	m_apoapseClient->OnDragFiles(files);
}
