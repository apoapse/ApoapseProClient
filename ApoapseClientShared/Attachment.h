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
#include "Username.h"
#include "Json.hpp"
#include "Uuid.h"
#include "DateTimeUtils.h"
class ApoapseClient;

class Attachment
{
	ApoapseClient& apoapseClient;
	
public:
	struct File
	{
		Uuid uuid;
		std::string fileName;
		std::string filePath;
		size_t fileSize = 0;
		bool attached = false;
		bool isDownloaded = false;
		bool isAvailable = false;
		std::optional<DbId> temporaryId;	// Used when the attachments have not been sent yet

		DataStructure GetDataStructure() const;

		File() = default;
		File(const std::string& filePath);
	};

	File relatedFile;
	DbId id = -1;
	Uuid parentMessage;
	Username sender;
	DateTimeUtils::UTCDateTime sentTime;

	//Attachment() = default;
	Attachment(const File& file, ApoapseClient& client);
	Attachment(DataStructure& data,  ApoapseClient& client);
	
	void RequestOpenFile();
	void SetFileAsDownloaded(bool autoOpen);
	void SetFileAsAvailable();
	void CopyFileLocally(const std::string& localFilePath);
	JsonHelper GetJson() const;

	static std::string GetAttachmentFilePath(const std::string& attDirectory, const Username& username, const Uuid& attUuid, const std::string& fileFullName);
};
