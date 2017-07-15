#pragma once
#include "include/cef_client.h"
#include <list>

class GenericHandler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler
{
	// List of existing browser windows. Only accessed on the CEF UI thread.
	using BrowserList = std::list<CefRefPtr<CefBrowser>>;
	BrowserList m_browserList;

	bool m_isClosing;

public:
	explicit GenericHandler();
	virtual ~GenericHandler() override;

	// Provide access to the single global instance of this object.
	static GenericHandler* GetInstance();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
	{
		return this;
	}

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
	{
		return this;
	}

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
	{
		return this;
	}

	// CefDisplayHandler methods:
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame, ErrorCode errorCode,	const CefString& errorText,	const CefString& failedUrl) override;

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);

	bool IsClosing() const
	{
		return m_isClosing;
	}

private:
	void PlatformTitleChange(CefRefPtr<CefBrowser> browser,	const CefString& title);

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(GenericHandler);
};