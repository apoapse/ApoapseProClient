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

namespace NativeUI
{
	std::string OpenFileDialog(const std::vector<std::string>& extensionsSupported, const std::string& extensionsGroup);
	std::string GetUserDirectory();
	void SystemOpenFile(const std::string& filePath);
	void SystemOpenURL(const std::string& url);
}