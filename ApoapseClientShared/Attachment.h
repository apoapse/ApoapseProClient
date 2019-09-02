#pragma once
#include "Username.h"
#include "Json.hpp"
#include "Uuid.h"
class ApoapseClient;

class Attachment
{
	ApoapseClient& apoapseClient;
	
public:
	struct File
	{
		std::string fileName;
		std::string filePath;
		size_t fileSize = 0;
		bool attached = false;
		bool isDownloaded = false;

		DataStructure GetDataStructure() const;

		File() = default;
		File(const std::string& filePath);
	};

	File relatedFile;
	DbId id = -1;
	Uuid uuid;
	Username sender;

	//Attachment() = default;
	Attachment(const File& file, ApoapseClient& client);
	Attachment(DataStructure& data, ApoapseClient& client);

	JsonHelper GetJson() const;

	static std::string GetAttachmentFilePath(const Username& username, const Uuid& attUuid, const std::string& fileFullName);
};
