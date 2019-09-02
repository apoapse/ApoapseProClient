#include "stdafx.h"
#include "Common.h"
#include "Attachment.h"
#include <filesystem>
#include "Uuid.h"
#include "ApoapseClient.h"
#include "HTMLUI.h"

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

Attachment::Attachment(const File& file, ApoapseClient& client) : relatedFile(file), apoapseClient(client)
{
}

Attachment::Attachment(DataStructure& data, ApoapseClient& client) : apoapseClient(client)
{
	uuid = data.GetField("uuid").GetValue<Uuid>();
	
	File file;
	file.fileName = data.GetField("name").GetValue<std::string>();
	file.fileSize = data.GetField("file_size").GetValue<Int64>();
	file.filePath = GetAttachmentFilePath(apoapseClient.GetLocalUser().GetUsername(), uuid, file.fileName);
	
	file.isDownloaded = (data.GetField("is_downloaded").HasValue()) ? data.GetField("is_downloaded").GetValue<bool>() : false;

	relatedFile = file;
	id = data.GetDbId();
}

JsonHelper Attachment::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("fileName", HTMLUI::HtmlSpecialChars(relatedFile.fileName, true));
	ser.Insert("fileSize", relatedFile.fileSize / 1000);	// In kb
	ser.Insert("isDownloaded", relatedFile.isDownloaded);
		
	return ser;
}

std::string Attachment::GetAttachmentFilePath(const Username& username, const Uuid& attUuid, const std::string& fileFullName)
{
	const std::string uuidStr = BytesToHexString(attUuid.GetBytes());
	return "client_download" + username.ToStr() + '/' + uuidStr.substr(0, 4) + fileFullName;
}
