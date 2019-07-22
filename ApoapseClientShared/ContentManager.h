#pragma once
#include "TypeDefs.hpp"
class ApoapseClient;

class ContentManager
{
	ApoapseClient& client;

public:
	ContentManager(ApoapseClient& apoapseClient);
};