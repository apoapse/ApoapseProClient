#pragma once
#include "TCPConnection.h"
#include "Common.h"

class DummyConnection : public TCPConnection
{
	

public:
	DummyConnection(boost::asio::io_service& io_service) : TCPConnection(io_service)
	{
	}
	
private:
protected:
	virtual bool OnConnectedToServer() override
	{
		LOG_DEBUG_FUNCTION_NAME();

		return true;
	}


	virtual bool OnReceivedError(const boost::system::error_code& error) override
	{
		LOG << "Error " << error.message();

		return true;
	}

};