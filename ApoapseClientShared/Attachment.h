#pragma once

class Attachment
{
public:
	struct File
	{
		std::string fileName;
		std::string filePath;
		size_t fileSize = 0;
		bool attached = false;

		DataStructure GetDataStructure() const;

		File() = default;
		File(const std::string& filePath);
	};

	File relatedFile;

	Attachment() = default;
	Attachment(const File& file);
	
};