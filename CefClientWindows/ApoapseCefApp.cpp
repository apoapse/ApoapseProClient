#include "ApoapseCefApp.h"
#include "simple_handler.h"

#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/views/cef_browser_view.h>
#include <include/views/cef_window.h>
#include <include/wrapper/cef_helpers.h>

#include "ApoapseSchemeHandler.h"

void ApoapseCefApp::OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine)
{
	//commandLine->AppendSwitchWithValue("url", "about:credits");
	commandLine->AppendSwitchWithValue("default-background-color", "ff0000ff");

}

// class SimpleWindowDelegate : public CefWindowDelegate
// {
// 	CefRefPtr<CefBrowserView> m_browserView;
// 
// public:
// 	explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browserView) : m_browserView(browserView)
// 	{
// 	}
// 
// 	void OnWindowCreated(CefRefPtr<CefWindow> window) override
// 	{
// 		// Add the browser view and show the window.
// 		window->AddChildView(m_browserView);
// 		window->Show();
// 
// 		// Give keyboard focus to the browser view.
// 		m_browserView->RequestFocus();
// 	}
// 
// 	void OnWindowDestroyed(CefRefPtr<CefWindow> window) override
// 	{
// 		m_browserView = nullptr;
// 	}
// 
// 	bool CanClose(CefRefPtr<CefWindow> window) override
// 	{
// 		CefRefPtr<CefBrowser> browser = m_browserView->GetBrowser();
// 
// 		if (browser)
// 			return browser->GetHost()->TryCloseBrowser();
// 		return true;
// 	}
// 
// private:
// 	IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
// 	DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
// };


class ApoapseSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
	ApoapseSchemeHandlerFactory() = default;

	// Return a new scheme handler instance to handle the request.
	CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame,	const CefString& scheme_name,	CefRefPtr<CefRequest> request) override
	{
		CEF_REQUIRE_IO_THREAD();
		return new ApoapseSchemeHandler();
	}

private:
	IMPLEMENT_REFCOUNTING(ApoapseSchemeHandlerFactory);
	DISALLOW_COPY_AND_ASSIGN(ApoapseSchemeHandlerFactory);
};


void ApoapseCefApp::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();

	CefRegisterSchemeHandlerFactory("http", "apoapse", new ApoapseSchemeHandlerFactory()); // #TODO Add support to check every single http and https request

	//CefRefPtr<CefCommandLine> commandLine = CefCommandLine::GetGlobalCommandLine();

	// SimpleHandler implements browser-level callbacks.
	const bool useView = false;
	CefRefPtr<GenericHandler> handler(new GenericHandler);

	CefBrowserSettings browserSettings;

	// Check if a "--url=" value was provided via the command-line. If so, use
	// that instead of the default URL.
// 	std::string url = commandLine->GetSwitchValue("url");
// 	if (url.empty())
// 		url = "http://www.google.com";

	std::string url = "http://apoapse/";

	// Information used when creating the native window.
	CefWindowInfo windowInfo;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	windowInfo.SetAsPopup(NULL, "Apoapse Pro");
#endif
	// Create the first browser window.
	CefBrowserHost::CreateBrowser(windowInfo, handler, url, browserSettings, NULL);
}