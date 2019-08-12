#include "stdafx.h"
#include "Common.h"
#include "ClientFileStreamConnection.h"
#include "Random.hpp"
#include "CryptographyTypes.hpp"

ClientFileStreamConnection::ClientFileStreamConnection(boost::asio::io_service& ioService, ssl::context& context, ApoapseClient& client)
	: FileStreamConnection(ioService, context)
{
	
}

void ClientFileStreamConnection::OnFileDownloadCompleted()
{
	LOG_DEBUG << "Download completed";
}

void ClientFileStreamConnection::ErrorDisconnectAll()
{
	LOG_DEBUG << "ErrorDisconnectAll";
	
}

void ClientFileStreamConnection::Authenticate(const Username& username, const hash_SHA256& authCode)
{
	LOG_DEBUG << "Authenticate";
}

std::string ClientFileStreamConnection::GetDownloadFilePath(UInt64 fileSize)
{
	LOG_DEBUG << "GetDownloadFilePath";
	//return "_RECTEST.jpg";
	return "_RECTEST.mkv";
}

void ClientFileStreamConnection::OnFileSentSuccessfully()
{
	LOG_DEBUG << "OnFileSentSuccessfully";
	
}

void ClientFileStreamConnection::OnConnectedToServer()
{
	LOG_DEBUG << "OnConnectedToServer";

	auto test = std::make_shared<ByteContainer>(Cryptography::GenerateRandomBytes(4 + sha256Length + sha256Length));
	Send(test);
}
