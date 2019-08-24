#include "stdafx.h"
#include "Common.h"
#include "Attachment.h"
#include <filesystem>

Attachment::File::File(const std::string& filePath)
{
	this->filePath = filePath;
	fileName = std::filesystem::path(filePath).filename().string();
	fileSize = std::filesystem::file_size(filePath);
}

Attachment::Attachment(const File& file) : relatedFile(file)
{
}
