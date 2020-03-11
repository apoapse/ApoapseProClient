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
#include <include/cef_app.h>
#include "ISignalSender.hpp"

class ApoapseCefApp : public CefApp, public CefBrowserProcessHandler, public ISignalSender
{
	CefRefPtr<CefBrowser> m_browser;
	HWND m_hwnd = 0;

public:
	ApoapseCefApp();

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
	{
		return this;
	}

	virtual void OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine) override;
	virtual void OnContextInitialized() override;

	virtual void SendSignal(const std::string& name, const std::string& data) override;

	void SetMainWindowsHwnd(HWND hwnd);

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ApoapseCefApp);
};