#pragma once
#include <set>
#include "INetworkSender.h"
class Uuid;

class ClientOperations
{
	std::set<Uuid> m_operations;
	Int64 m_lastOperationTime = 1;

public:
	ClientOperations();

	bool IsAlreadyRegistered(const Uuid& opUuid) const;
	void RegisterOperationUuid(const Uuid& uuid);

	Int64 GetMostRecentOpTime() const;
	void SendSyncRequest(INetworkSender& destination) const;
};