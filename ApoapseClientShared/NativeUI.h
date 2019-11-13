#pragma once

namespace NativeUI
{
	std::string OpenFileDialog(const std::vector<std::string>& extensionsSupported, const std::string& extensionsGroup);
	std::string GetUserDirectory();
	void SystemOpenFile(const std::string& filePath);
	void SystemOpenURL(const std::string& url);
}