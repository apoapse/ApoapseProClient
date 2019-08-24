#pragma once

class Attachment
{
public:
	struct File
	{
		std::string fileName;
		std::string filePath;
		size_t fileSize = 0;

		File() = default;
		File(const std::string& filePath);
	};

	File relatedFile;

	Attachment(const File& file);
	
};