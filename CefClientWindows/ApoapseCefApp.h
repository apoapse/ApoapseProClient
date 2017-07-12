#pragma once
#include <include/cef_app.h>

// Implement application-level callbacks for the browser process.
class ApoapseCefApp : public CefApp, public CefBrowserProcessHandler {
public:
	ApoapseCefApp() = default;

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
		OVERRIDE {
		return this;
	}

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ApoapseCefApp);
};