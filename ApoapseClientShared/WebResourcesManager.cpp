#include "stdafx.h"
#include "WebResourcesManager.h"
#include "Common.h"
#include <filesystem>
#include "FileUtils.h"
#include "StringExtensions.h"

std::vector<byte> WebResourcesManager::ReadFile(const std::string& filename, const std::string& fileExtension)
{
	std::string filePath = "ClientResources/" + filename;

	if (StringExtensions::contains(filename, "client_avatar"))
	{
		filePath = "./" + filename;
	}

	if (!std::filesystem::exists(filePath))
	{
		LOG << "Unable to load the file: " << filePath << '\n' << LogSeverity::warning;
		return std::vector<byte>();
	}

	return FileUtils::ReadFile(filePath);
}
