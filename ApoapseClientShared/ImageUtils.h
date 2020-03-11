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

namespace ImageUtils
{
	std::vector<byte> ReadAndResizeImage(const std::string& filePath, int newWidth, int newHeight, bool preserveRatio = false, const std::string& forceEncoding = std::string());
}