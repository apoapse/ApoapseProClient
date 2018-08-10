#pragma once
#include "Uuid.h"
#include "ApoapseThread.h"
class ApoapseClient;
struct SimpleApoapseThread;

struct ApoapseRoom
{
	DbId dbId = -1;
	Uuid uuid;
	std::vector<SimpleApoapseThread> threads;
};

class RoomManager
{
	std::vector<std::unique_ptr<ApoapseRoom>> m_rooms;
	ApoapseClient& apoapseClient;
	ApoapseRoom* m_uiSelectedRoom = nullptr;

public:
	RoomManager(ApoapseClient& client);
	void Initialize();
	void SendCreateNewRoom(const std::string& name);
	void AddNewRoomFromServer(std::unique_ptr<ApoapseRoom> room);
	void SetUISelectedRoom(UInt64 internalRoomId);
	const ApoapseRoom* GetSelectedRoom() const;
	ApoapseRoom* GetRoomByUuid(const Uuid& uuid) const;

	void SendAddNewThread(const std::string& name);
	void AddNewThreadFromServer(const Uuid& uuid, const Uuid& roomUuid, const std::string& name);
	//virtual ~RoomManager();
	
private:
	void OnNewThreadAddedToCurrentRoom(SimpleApoapseThread& thread);
	void LoadThreadsLists();

	void UpdateThreadListUI();
	void UpdateUI();
};