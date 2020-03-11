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
#include "ClientOperations.h"
#include "Uuid.h"
#include "DateTimeUtils.h"
#include "CommandsManagerV2.h"

ClientOperations::ClientOperations()
{
	auto operations = global->apoapseData->ReadListFromDatabase("operation", "", "", "time", ResultOrder::asc);
	for (auto& operation : operations)
	{
		m_operations.insert(operation.GetField("uuid").GetValue<Uuid>());
	}

	if (!operations.empty())
	{
		m_lastOperationTime = operations.at(operations.size() - 1).GetField("time").GetValue<Int64>();	// The most recent timestamp is the last item on the list
	}

	LOG << "Loaded " << m_operations.size() << " operations";
}

bool ClientOperations::IsAlreadyRegistered(const Uuid& opUuid) const
{
	return (m_operations.find(opUuid) != m_operations.end());
}

void ClientOperations::RegisterOperationUuid(const Uuid& uuid)
{
	m_lastOperationTime = DateTimeUtils::UnixTimestampNow();
	m_operations.insert(uuid);
}

Int64 ClientOperations::GetMostRecentOpTime() const
{
	return m_lastOperationTime;
}

void ClientOperations::SendSyncRequest(INetworkSender& destination) const
{
	DataStructure data = global->apoapseData->GetStructure("sync");
	data.GetField("last_op_time").SetValue(GetMostRecentOpTime());

	global->cmdManager->CreateCommand("request_sync", data).Send(destination);
}
