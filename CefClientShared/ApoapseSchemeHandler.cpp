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
	
	//DLOG(INFO) << "MOTHOD: " << method << " URL: " << fullUrl << " fileExtension:" << fileExtension;

	if (!fileExtension.empty())
	{
		// Data
		SetMimeFromExtension(fileExtension);
		m_responseData = ApoapseClientEntry::ReadFile("ClientResources/" + ReadFileName(fullUrl), fileExtension);
	}
	else
	{
		// Signal
		std::string postData{};

		if (request->GetPostData().get() != nullptr)
		{
			CefPostData::ElementVector postElements;
			request->GetPostData()->GetElements(postElements);

			for (const auto& element : postElements)
			{
				const size_t bytesCount = element->GetBytesCount();
				byte* bytes = new byte[bytesCount];
				element->GetBytes(bytesCount, bytes);

				postData = std::string(bytes, bytes + bytesCount);
				delete[] bytes;
			}
		}

		std::string returnData = ApoapseClientEntry::OnReceivedSignal(ReadSignalName(fullUrl), postData);
		m_responseData = std::vector<byte>(returnData.begin(), returnData.end());

		SetMimeFromExtension("");
	}

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

std::string ApoapseSchemeHandler::ReadSignalName(const std::string& path)
{
	static const std::string basePath = "http://apoapse/signal/";

	return path.substr(basePath.length(), path.length());
}

