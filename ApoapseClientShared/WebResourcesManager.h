#pragma once
#include "TypeDefs.hpp"
#include <vector>
#include "DllExportSymbolApi.hpp"

class WebResourcesManager
{
	

public:
	//WebResourcesManager();

	std::vector<byte> ReadFile(const std::string& filename, const std::string& fileExtension);
	
private:
};