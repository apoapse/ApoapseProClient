#pragma once
#include "Uuid.h"
class ApoapseClient;

struct ApoapseRoom
{
	Uuid uuid;
	//std::vector<ApoapseThread> threads;
	
};

class RoomManager
{
	std::vector<ApoapseRoom> m_rooms;
	ApoapseClient& apoapseClient;

public:
	RoomManager(ApoapseClient& client);
	void Initialize();
	void SendCreateNewRoom(const std::string& name);
	void AddNewRoomFromServer(const ApoapseRoom& room);

	//virtual ~RoomManager();
	
private:
	void UpdateUI();
};