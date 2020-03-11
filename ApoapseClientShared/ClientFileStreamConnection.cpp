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
#include "Common.h"
#include "ClientFileStreamConnection.h"
#include "Random.hpp"
#include "CryptographyTypes.hpp"
#include "ApoapseClient.h"
#include "ClientConnection.h"
#include "Attachment.h"

ClientFileStreamConnection::ClientFileStreamConnection(boost::asio::io_service& ioService, ssl::context& context, ApoapseClient& client)
	: FileStreamConnection(ioService, context)
	, client(client)
{
	
}

void ClientFileStreamConnection::OnFileDownloadCompleted(const AttachmentFile& file)
{
	LOG << "Download completed";

	global->mainThread->PushTask([this, file]()
	{
		std::shared_ptr<Attachment> att = client.GetContentManager().GetAttachment(file.uuid);
		att->SetFileAsDownloaded(true);
	});
}

void ClientFileStreamConnection::ErrorDisconnectAll()
{
	LOG_DEBUG << "ErrorDisconnectAll";

	client.GetConnection()->Close();
	Close();
}

void ClientFileStreamConnection::OnFileSentSuccessfully(const AttachmentFile& file)
{
	LOG << "File " << file.fileName << " sent successfully";

	global->mainThread->PushTask([this]()
	{
		client.OnFileUploaded();
	});
}

void ClientFileStreamConnection::OnSocketConnected()
{
	LOG_DEBUG << "OnSocketConnected";
}
