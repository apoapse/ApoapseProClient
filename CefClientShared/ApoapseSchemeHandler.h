#pragma once
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <memory>
#include <include/cef_resource_handler.h>

class ApoapseSchemeHandler : public CefResourceHandler
{
	std::optional<std::unique_ptr<std::string>> m_responseData;
	//Range<std::string> m_responseWorkingRange;

public:
	ApoapseSchemeHandler() = default;

	virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
	virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;

	virtual void Cancel() override;

	void PrepareResponse(std::unique_ptr<std::string> data);
	//void PrepareResponse(const std::vector<byte>* data);

private:
	IMPLEMENT_REFCOUNTING(ApoapseSchemeHandler);
	DISALLOW_COPY_AND_ASSIGN(ApoapseSchemeHandler);
};