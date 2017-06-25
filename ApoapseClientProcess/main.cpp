#include "stdafx.h"
#include "Common.h"
#include "GenericConnection.h"//TEMP

#ifdef UNIT_TESTS
#include "UnitTestsManager.h"
#endif //UNIT_TESTS

int main(int argcount, char* argv[])
{
	std::vector<std::string> launchArgs(argv, argv + argcount);

	// Initialize global systems
	ASSERT(global == nullptr);
	global = Global::CreateGlobal();

	global->logger = std::make_unique<Logger>("log.txt");
	global->threadPool = std::make_unique<ThreadPool>("Global thread pool", 8); // #TODO dynamically choose the number of threads into the global thread pool

	// Run unit tests if requested
#ifdef UNIT_TESTS
	if (std::find(launchArgs.begin(), launchArgs.end(), "-run_unit_tests") != launchArgs.end())
	{
		UnitTestsManager::GetInstance().RunTests();
		return 0;
	}
#endif //UNIT_TESTS

	{
		boost::asio::io_service ioService;
		auto connection = std::make_shared<GenericConnection>(ioService);
		connection->Connect("127.0.0.1", 5700);

		auto test = std::vector<byte> { 'a', 'b', 'c', 'd' };
		connection->Send(IdsCommand::CONNECT, std::make_unique<std::vector<byte>>(test), nullptr);
		ioService.run();
	}

	std::string inputstr;
	getline(std::cin, inputstr);

	delete global;
	return 0;
}