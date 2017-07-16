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

bool ApoapseSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
	CEF_REQUIRE_IO_THREAD();

	std::string method = request->GetMethod();
	std::string fullUrl = request->GetURL();
	
	DLOG(INFO) << "MOTHOD: " << method << " URL: " << fullUrl;

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

			DLOG(INFO) << "POST data: " << str;
		}
	}

	PrepareResponse(std::make_unique<std::string>("WORKS!<script>xmlhttp = new XMLHttpRequest();    xmlhttp.open('POST', 'http://apoapse/action/', true);    xmlhttp.send('var=1');</script>"));

	callback->Continue();

	return true;
}

void ApoapseSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
	CEF_REQUIRE_IO_THREAD();

	if (m_responseData)
	{
		response_length = m_responseData.value()->size();

		response->SetMimeType("text/html");
		response->SetStatus(200);
	}
	else
	{
		response->SetStatus(404);
	}
}

bool ApoapseSchemeHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
	if (m_responseData)
	{
		memcpy(data_out, m_responseData.value()->data(), m_responseData.value()->size());
		bytes_read = bytes_to_read;

		m_responseData.reset();
	}

	return true;
}

void ApoapseSchemeHandler::Cancel()
{
	CEF_REQUIRE_IO_THREAD();
}

void ApoapseSchemeHandler::PrepareResponse(std::unique_ptr<std::string> data)
{
	ASSERT(!m_responseData.has_value());

	m_responseData = std::move(data);
}
