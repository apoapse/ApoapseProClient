#include "stdafx.h"
#include "Common.h"
#include "GenericConnection.h"//TEMP
#include "CommandsManager.h"//temp
#include "NetworkPayload.h"

#ifdef UNIT_TESTS
#include "UnitTestsManager.h"
#endif //UNIT_TESTS

int main(int argcount, char* argv[])
{
	std::vector<std::string> launchArgs(argv, argv + argcount);

	// Initialize global systems
	ASSERT(global == nullptr);
	global = Global::CreateGlobal();

	global->logger = std::make_unique<Logger>("log_client.txt");
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
		MessagePackSerializer serializer;
		std::vector<int> arrayData{ 6, 7, 8, -625 };

		serializer.Group("",
		{
			MSGPK_ORDERED_APPEND(serializer, "user", "guillaume"),
			MSGPK_ORDERED_APPEND(serializer, "pass", "ddqsdqzdssfzsedqsd5sdf8se9sf5"),
			MSGPK_ORDERED_APPEND_ARRAY(serializer, int, "test_array", arrayData),

			//[&] { serializer.OrderedAppendArray<int>("test_array", std::vector<int> { 1, 2, 3 }); }
		});

		auto payloadBody = serializer.GetMessagePackBytes();
		auto header = NetworkPayload::GenerateHeader(Commands::CONNECT, payloadBody);
		std::vector<byte> data(header.begin(), header.end());
		data.insert(data.end(), payloadBody.begin(), payloadBody.end());

		auto payload = std::make_shared<NetworkPayload>();
		auto cmd = CommandsManager::GetInstance().CreateCommand(Commands::CONNECT);

		{
			std::array<byte, READ_BUFFER_SIZE> arr;
			std::copy(data.begin(), data.end(), arr.begin());

			Range<std::array<byte, READ_BUFFER_SIZE>> range(arr, data.size());
			payload->Insert(range);

			ASSERT(payload->headerInfo->command == Commands::CONNECT);
		}
		
		cmd->Parse(payload);
	}

	{
		boost::asio::io_service ioService;
		auto connection = std::make_shared<GenericConnection>(ioService);
		connection->Connect("127.0.0.1", 5700);

		auto test = std::vector<byte> { 'a', 'b', 'c', 'd' };
		connection->Send(Commands::CONNECT, std::make_unique<std::vector<byte>>(test), nullptr);
		ioService.run();
	}

	std::string inputstr;
	getline(std::cin, inputstr);

	delete global;
	return 0;
}