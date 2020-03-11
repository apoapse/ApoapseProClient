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
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <memory>
#include <include/cef_resource_handler.h>
#include <include/cef_scheme.h>

class ApoapseSchemeHandler : public CefResourceHandler
{
	std::optional<std::vector<byte>> m_responseData;
	int m_responseReadOffset = 0;
	std::string m_mime;

public:
	ApoapseSchemeHandler() = default;

	virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
	virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

	virtual void Cancel() override;

private:
	void SetMimeFromExtension(const std::string fileExtension);
	static std::string ReadFileExtension(const std::string& path);
	static std::string ReadFileName(const std::string& path);
	static std::string ReadSignalName(const std::string& path);

	IMPLEMENT_REFCOUNTING(ApoapseSchemeHandler);
	DISALLOW_COPY_AND_ASSIGN(ApoapseSchemeHandler);
};

class ApoapseSchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
	ApoapseSchemeHandlerFactory() = default;

	// Return a new scheme handler instance to handle the request.
	CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) override
	{
		return new ApoapseSchemeHandler;
	}

private:
	IMPLEMENT_REFCOUNTING(ApoapseSchemeHandlerFactory);
	DISALLOW_COPY_AND_ASSIGN(ApoapseSchemeHandlerFactory);
};