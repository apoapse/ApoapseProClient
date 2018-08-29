#pragma once
#include "Uuid.h"
#include <optional>
#include "ApoapseThread.h"
class ApoapseClient;

struct ApoapseRoom
{
	DbId dbId = -1;
	Uuid uuid;
	UIId uiId = -1;
	std::string name;
	std::vector<SimpleApoapseThread> threads;
};

class RoomManager
{
	std::vector<std::unique_ptr<ApoapseRoom>> m_rooms;
	ApoapseRoom* m_uiSelectedRoom = nullptr;
	std::optional<ApoapseThread> m_selectedThread;

public:
	ApoapseClient& apoapseClient;

	RoomManager(ApoapseClient& client);
	void Initialize();
	void SendCreateNewRoom(const std::string& name);
	void AddNewRoomFromServer(std::unique_ptr<ApoapseRoom> room);
	void SetUISelectedRoom(UInt64 internalRoomId);
	const ApoapseRoom* GetSelectedRoom() const;
	ApoapseRoom* GetRoomByUuid(const Uuid& uuid) const;
	void SetActiveThread(Int64 id);

	void SendAddNewThread(const std::string& name);
	void AddNewThreadFromServer(const Uuid& uuid, const Uuid& roomUuid, const std::string& name);
	ApoapseThread* GetActiveThread();
	SimpleApoapseThread* GetThreadByUuid(const Uuid& uuid);
	//virtual ~RoomManager();
	
private:
	void OnNewThreadAddedToCurrentRoom(SimpleApoapseThread& thread, UIId uiId);
	void LoadThreadsLists();

	void UpdateThreadListUI() const;
	void UpdateUI() const;
};