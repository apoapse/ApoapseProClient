#pragma once
#include "FileStreamConnection.h"
class ApoapseClient;

class ClientFileStreamConnection : public FileStreamConnection
{
public:
	ClientFileStreamConnection(boost::asio::io_service& ioService, ssl::context& context, ApoapseClient& client);
protected:
	void OnFileDownloadCompleted() override;
	void ErrorDisconnectAll() override;
	void Authenticate(const Username& username, const hash_SHA256& authCode) override;
	std::string GetDownloadFilePath(UInt64 fileSize) override;
	void OnFileSentSuccessfully() override;
	void OnConnectedToServer() override;
};