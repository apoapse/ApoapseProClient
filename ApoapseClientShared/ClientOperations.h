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