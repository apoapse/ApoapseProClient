#include "ApoapseSchemeHandler.h"
#include "Diagnostic.hpp"
#include "TypeDefs.hpp"

#include <include/cef_browser.h>
#include <include/cef_callback.h>
#include <include/cef_frame.h>
#include <include/cef_request.h>
#include <include/cef_response.h>
#include <include/cef_scheme.h>
#include <include/wrapper/cef_helpers.h>

#include "ClientEntryPoint.h"

bool ApoapseSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	CEF_REQUIRE_IO_THREAD();

	const std::string method = request->GetMethod();
	const std::string fullUrl = request->GetURL();
	const std::string fileExtension = ReadFileExtension(fullUrl);
	
	DLOG(INFO) << "MOTHOD: " << method << " URL: " << fullUrl << " fileExtension:" << fileExtension;

	if (!fileExtension.empty())
	{
		SetMimeFromExtension(fileExtension);
		m_responseData = ApoapseClient::ReadFile("ClientResources/" + ReadFileName(fullUrl), fileExtension);
	}
	else
	{
		SetMimeFromExtension("");
	}

	if (request->GetPostData().get() != nullptr)
	{
		CefPostData::ElementVector postElements;
		request->GetPostData()->GetElements(postElements);

		for (const auto& element : postElements)
		{
			const size_t bytesCount = element->GetBytesCount();
			byte* bytes = new unsigned char[bytesCount];
			element->GetBytes(bytesCount, bytes);

			//std::vector<byte> arr(bytes, bytes + bytesCount);

			std::string str(bytes, bytes + bytesCount);
			delete[] bytes;
			DLOG(INFO) << "POST data: " << str;
		}
	}

	//PrepareResponse(std::make_unique<std::string>("WORKS!<script>xmlhttp = new XMLHttpRequest();    xmlhttp.open('POST', 'http://apoapse/action/', true);    xmlhttp.send('var=1');</script>"));

	callback->Continue();

	return true;
}

void ApoapseSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
	CEF_REQUIRE_IO_THREAD();

	if (m_responseData.has_value())
	{
		response_length = m_responseData->size();

		response->SetMimeType(m_mime);
		response->SetStatus(200);
	}
	else
	{
		response->SetStatus(404);
	}
}

bool ApoapseSchemeHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	if (m_responseData.has_value())
	{
		memcpy(data_out, m_responseData->data() + m_responseReadOffset, bytes_to_read);
		bytes_read = bytes_to_read;

		if (bytes_read < m_responseData->size())
		{
			m_responseReadOffset += bytes_read;
		}
		else
		{
			m_responseData.reset();
		}
	}

	return true;
}

void ApoapseSchemeHandler::Cancel()
{
	CEF_REQUIRE_IO_THREAD();
}

void ApoapseSchemeHandler::PrepareResponse(std::unique_ptr<std::string> data)
{
	//ASSERT(!m_responseData.has_value());

	//m_responseData = std::move(data);
}

void ApoapseSchemeHandler::SetMimeFromExtension(const std::string fileExtension)
{
	m_mime = "text/html";	// Default mime

	if (fileExtension == "png")
		m_mime = "image/png";

	else if (fileExtension == "js")
		m_mime = "application/javascript";

	if (fileExtension == "jpg")
		m_mime = "image/jpeg";
}

std::string ApoapseSchemeHandler::ReadFileExtension(const std::string& path)
{
	std::string output{};

	const size_t dotPos = path.find_last_of('.');
	if (dotPos != std::string::npos)
	{
		output = path.substr(dotPos + 1, path.length());
	}

	return output;
}

std::string ApoapseSchemeHandler::ReadFileName(const std::string& path)
{
	static const std::string basePath = "http://apoapse/resources/";

	return path.substr(basePath.length(), path.length());
}

