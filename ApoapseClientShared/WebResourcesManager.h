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