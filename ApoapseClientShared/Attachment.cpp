#include "stdafx.h"
#include "Common.h"
#include "Attachment.h"
#include <filesystem>
#include "Uuid.h"
#include "ApoapseClient.h"
#include "HTMLUI.h"
#include "ClientConnection.h"
#include "ClientFileStreamConnection.h"
#include "NativeUI.h"

DataStructure Attachment::File::GetDataStructure() const
{
	DataStructure dat = global->apoapseData->GetStructure("attachment");
	dat.GetField("uuid").SetValue(uuid);
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
	File file;
	file.uuid = data.GetField("uuid").GetValue<Uuid>();
	file.fileName = data.GetField("name").GetValue<std::string>();
	file.fileSize = data.GetField("file_size").GetValue<Int64>();
	file.filePath = GetAttachmentFilePath(apoapseClient.GetAttachmentsDirectory(), apoapseClient.GetLocalUser().GetUsername(), file.uuid, file.fileName);
	file.isDownloaded = (data.GetField("is_downloaded").HasValue()) ? data.GetField("is_downloaded").GetValue<bool>() : false;
	file.isAvailable = data.GetField("is_available").GetValue<bool>();
	relatedFile = file;
	
	parentMessage = data.GetField("parent_message").GetValue<Uuid>();

	{
		DataStructure msgDat = global->apoapseData->ReadItemFromDBCustomFields("message", "uuid", parentMessage, std::vector<std::string>{"author", "sent_time"});
		sender = msgDat.GetField("author").GetValue<Username>();
		sentTime = msgDat.GetField("sent_time").GetValue<DateTimeUtils::UTCDateTime>();
	}

	id = data.GetDbId();
}

void Attachment::RequestOpenFile()
{
	const std::string filePath = GetAttachmentFilePath(apoapseClient.GetAttachmentsDirectory(), apoapseClient.GetLocalUser().GetUsername(), relatedFile.uuid, relatedFile.fileName);

	if (relatedFile.isDownloaded)
	{
		if (std::filesystem::exists(filePath))
		{
			LOG << "Opening attachment file " << filePath;
			NativeUI::SystemOpenFile(filePath);
		}
		else
		{
			LOG << LogSeverity::warning << "The file " << relatedFile.fileName << " should be saved locally but cannot be found. Downloading again.";
			relatedFile.isDownloaded = false;
			RequestOpenFile();
		}
	}
	else if (relatedFile.isAvailable)
	{
		{
			AttachmentFile file;
			file.uuid = relatedFile.uuid;
			file.fileName = relatedFile.fileName;
			file.filePath = filePath;
			file.fileSize = relatedFile.fileSize;
			
			apoapseClient.GetFileStreamConnection()->PushFileToReceive(file);
		}

		{
			DataStructure dat = global->apoapseData->GetStructure("attachment_download");
			dat.GetField("uuid").SetValue(relatedFile.uuid);

			global->cmdManager->CreateCommand("attachment_download", dat).Send(*apoapseClient.GetConnection());
		}

		{
			JsonHelper ser;
			ser.Insert("id", id);
			ser.Insert("status", "downloading");

			global->htmlUI->SendSignal("ChangeAttachmentStatus", ser.Generate());
		}
	}
	else
	{
		LOG << LogSeverity::error << "The file " << relatedFile.fileName << " is not available on the server at this time.";
	}
}

void Attachment::SetFileAsDownloaded(bool autoOpen)
{
	relatedFile.isDownloaded = true;

	{
		auto dat = global->apoapseData->ReadItemFromDatabase("attachment", "uuid", relatedFile.uuid);
		dat.GetField("is_downloaded").SetValue(true);
		dat.SaveToDatabase();
	}
	
	if (autoOpen)
		RequestOpenFile();

	{
		JsonHelper ser;
		ser.Insert("id", id);
		ser.Insert("status", "ready");

		global->htmlUI->SendSignal("ChangeAttachmentStatus", ser.Generate());
	}
}

void Attachment::SetFileAsAvailable()
{
	relatedFile.isAvailable = true;

	{
		auto dat = global->apoapseData->ReadItemFromDatabase("attachment", "uuid", relatedFile.uuid);
		dat.GetField("is_available").SetValue(true);
		dat.SaveToDatabase();
	}
	
	{
		JsonHelper ser;
		ser.Insert("id", id);
		ser.Insert("status", "ready");

		global->htmlUI->SendSignal("ChangeAttachmentStatus", ser.Generate());
	}
}

void Attachment::CopyFileLocally(const std::string& localFilePath)
{
	const std::string finalPath = GetAttachmentFilePath(apoapseClient.GetAttachmentsDirectory(), apoapseClient.GetLocalUser().GetUsername(), relatedFile.uuid, relatedFile.fileName);
	const std::string filePathFolder = std::filesystem::path(finalPath).parent_path().string();
	
	// We create the parent directory if it does not exist
	if (!filePathFolder.empty() && !std::filesystem::exists(filePathFolder))
	{
		std::filesystem::create_directory(filePathFolder);
	}
	
	std::filesystem::copy(localFilePath, finalPath);
	
	SetFileAsDownloaded(false);
}

JsonHelper Attachment::GetJson() const
{
	JsonHelper ser;
	ser.Insert("id", id);
	ser.Insert("fileName", HTMLUI::HtmlSpecialChars(relatedFile.fileName));
	ser.Insert("fileSize", relatedFile.fileSize / 1000);	// In kb
	ser.Insert("isDownloaded", relatedFile.isDownloaded);
	ser.Insert("isAvailable", relatedFile.isAvailable);
	ser.Insert("author", HTMLUI::HtmlSpecialChars(apoapseClient.GetClientUsers().GetUserByUsername(sender).nickname));
	ser.Insert("dateTime", sentTime.GetStr());
		
	return ser;
}

std::string Attachment::GetAttachmentFilePath(const std::string& attDirectory, const Username& username, const Uuid& attUuid, const std::string& fileFullName)
{
	const std::string uuidStr = BytesToHexString(attUuid.GetBytes());
	return attDirectory + '/' + uuidStr.substr(0, 10) + '/' + fileFullName;
}