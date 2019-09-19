#pragma once
#include "FileStreamConnection.h"
class ApoapseClient;

class ClientFileStreamConnection : public FileStreamConnection
{
public:
	ApoapseClient& client;
	
	ClientFileStreamConnection(boost::asio::io_service& ioService, ssl::context& context, ApoapseClient& client);
	
protected:
	void OnFileDownloadCompleted(const AttachmentFile& file) override;
	void ErrorDisconnectAll() override;
	void OnFileSentSuccessfully(const AttachmentFile& file) override;
	void OnSocketConnected() override;
};