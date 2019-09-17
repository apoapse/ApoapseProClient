#pragma once
#include "TypeDefs.hpp"
#include <vector>
class ApoapseClient;

class WebResourcesManager
{
	ApoapseClient& m_apoapseClient;

public:
	WebResourcesManager(ApoapseClient& client);

	std::vector<byte> ReadFile(const std::string& filename, const std::string& fileExtension);
	
private:
};