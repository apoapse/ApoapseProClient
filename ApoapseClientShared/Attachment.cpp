#include "stdafx.h"
#include "Common.h"
#include "Attachment.h"
#include <filesystem>
#include "Uuid.h"

DataStructure Attachment::File::GetDataStructure() const
{
	DataStructure dat = global->apoapseData->GetStructure("attachment");
	dat.GetField("uuid").SetValue(Uuid::Generate());
	dat.GetField("name").SetValue(fileName);
	dat.GetField("file_size").SetValue((Int64)fileSize);

	return dat;
}

Attachment::File::File(const std::string& filePath)
{
	this->filePath = filePath;
	fileName = std::filesystem::path(filePath).filename().string();
	fileSize = std::filesystem::file_size(filePath);
}

Attachment::Attachment(const File& file) : relatedFile(file)
{
}
