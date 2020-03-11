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