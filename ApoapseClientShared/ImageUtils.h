#pragma once
#include "TypeDefs.hpp"

namespace ImageUtils
{
	std::vector<byte> ReadAndResizeImage(const std::string& filePath, int newWidth, int newHeight, bool preserveRatio = false, const std::string& forceEncoding = std::string());
}