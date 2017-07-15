#pragma once
#include <include/cef_resource_handler.h>

class ApoapseSchemeHandler : public CefResourceHandler
{

public:
	ApoapseSchemeHandler();

	virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
	virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

	virtual void Cancel() override;

private:
	IMPLEMENT_REFCOUNTING(ApoapseSchemeHandler);
	DISALLOW_COPY_AND_ASSIGN(ApoapseSchemeHandler);
};