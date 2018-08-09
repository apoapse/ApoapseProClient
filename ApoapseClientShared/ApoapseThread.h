#pragma once
#include "Username.h"

struct SimpleApoapseThread
{
	DbId dbId = -1;
	Uuid uuid;
	std::string name;
	Username lastMessageAuthor;
	std::string lastMessageText;
};

class ApoapseThread
{
	

public:
	ApoapseThread();
// 	virtual ~ApoapseThread();
	
private:
};