#pragma once
#include <include/cef_app.h>

class ApoapseCefApp : public CefApp, public CefBrowserProcessHandler
{
public:
	ApoapseCefApp();

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
	{
		return this;
	}

	virtual void OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine) override;

	virtual void OnContextInitialized() override;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ApoapseCefApp);
};