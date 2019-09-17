#include "stdafx.h"
#include "WebResourcesManager.h"
#include "Common.h"
#include <filesystem>
#include "FileUtils.h"
#include "StringExtensions.h"
#include "NativeUI.h"
#include "ApoapseClient.h"

WebResourcesManager::WebResourcesManager(ApoapseClient& client) : m_apoapseClient(client)
{
}

std::vector<byte> WebResourcesManager::ReadFile(const std::string& filename, const std::string& fileExtension)
{
	std::string filePath = "ClientResources/" + filename;

	if (StringExtensions::contains(filename, "client_avatar"))
	{
		filePath = NativeUI::GetUserDirectory() + filename;
	}

	if (!std::filesystem::exists(filePath))
	{
		LOG << "Unable to load the file: " << filePath << '\n' << LogSeverity::warning;
		return std::vector<byte>();
	}

	const std::vector<byte> res = FileUtils::ReadFile(filePath);

	if (filename == "main.html")
	{
		const std::string prefix = "<script>g_locale = '" + m_apoapseClient.clientSettings.ReadFieldValue<std::string>("language").value() + "';</script>";	// Global variables
		std::vector<byte> output(prefix.size() + res.size());
		
		std::copy(prefix.begin(), prefix.end(), output.begin());
		std::copy(res.begin(), res.end(), output.begin() + prefix.size());

		return output;
	}
	
	return res;
}
