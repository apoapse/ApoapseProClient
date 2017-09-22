#pragma once
#include <string>

struct ISignalSender
{
	//virtual ~ISignalSender() = default;

	virtual void SendSignal(const std::string& name, const std::string& data) = 0;
};