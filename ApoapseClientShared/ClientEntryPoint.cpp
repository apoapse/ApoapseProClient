#include "stdafx.h"
#include "ClientEntryPoint.h"
#include "Common.h"
#include "HTMLUI.h"
#include "ApoapseClient.h"
#include "DataStructures.hpp"
#include "ClientCmdManager.h"
#include "ThreadUtils.h"

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

		global->logger = std::make_unique<Logger>("log_client.txt");
		global->threadPool = std::make_unique<ThreadPool>("Global thread pool", 2, true);

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
	m_apoapseClient->OnDradFiles(files);
}
