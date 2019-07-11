#include "stdafx.h"
#include "ClientEntryPoint.h"
#include "Common.h"
#include "HTMLUI.h"
#include "ApoapseClient.h"
#include "DataStructures.hpp"
#include "ClientCmdManager.h"

#ifdef UNIT_TESTS
#include "UnitTestsManager.h"
#endif //UNIT_TESTS

ApoapseClient* m_apoapseClient = nullptr;

int ApoapseClientEntry::ClientMain(const std::vector<std::string>& launchArgs)
{
	// Initialize global systems
	{
		ASSERT(global == nullptr);
		global = Global::CreateGlobal();
		global->isClient = true;

		global->apoapseData = std::make_unique<ApoapseData>(GetDataStructures());
		global->cmdManager = std::make_unique<ClientCmdManager>();

		global->logger = std::make_unique<Logger>("log_client.txt");
		global->threadPool = std::make_unique<ThreadPool>("Global thread pool", 8); // #TODO dynamically choose the number of threads into the global thread pool

		m_apoapseClient = new ApoapseClient;
		global->htmlUI = new HTMLUI(*m_apoapseClient);
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

	return 1;
}

void ApoapseClientEntry::Shutdown()
{
	delete global->htmlUI;
	delete m_apoapseClient;
}

std::vector<byte> ApoapseClientEntry::ReadFile(const std::string& filename, const std::string& fileExtension)
{
	return global->htmlUI->GetWebResourcesManager().ReadFile(filename, fileExtension);
}

void ApoapseClientEntry::RegisterSignalSender(ISignalSender* signalSender)
{
	global->htmlUI->RegisterSignalSender(signalSender);
}

std::string ApoapseClientEntry::OnReceivedSignal(const std::string& name, const std::string& data)
{
	return global->htmlUI->OnReceivedSignal(name, data);
}
