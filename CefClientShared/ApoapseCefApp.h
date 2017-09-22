#pragma once
#include <include/cef_app.h>
#include "ISignalSender.hpp"

class ApoapseCefApp : public CefApp, public CefBrowserProcessHandler, public ISignalSender
{
	CefRefPtr<CefBrowser> m_browser;

public:
	ApoapseCefApp();

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
	{
		return this;
	}

	virtual void OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine) override;
	virtual void OnContextInitialized() override;

	virtual void SendSignal(const std::string& name, const std::string& data) override;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ApoapseCefApp);
};