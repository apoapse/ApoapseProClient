#include "ApoapseCefApp.h"
#include "GenericHandler.h"

#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/views/cef_browser_view.h>
#include <include/views/cef_window.h>
#include <include/wrapper/cef_helpers.h>

#include "ApoapseSchemeHandler.h"

void ApoapseCefApp::OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine)
{
	//commandLine->AppendSwitchWithValue("url", "about:credits");
	//commandLine->AppendSwitchWithValue("default-background-color", "ff0000ff");

}

class ApoapseSchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
	ApoapseSchemeHandlerFactory() = default;

	// Return a new scheme handler instance to handle the request.
	CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) override
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

	CefRegisterSchemeHandlerFactory("http", "apoapse", new ApoapseSchemeHandlerFactory());


	const bool useView = false;
	CefRefPtr<GenericHandler> handler(new GenericHandler);

	CefBrowserSettings browserSettings;

	const std::string url = "http://apoapse/hi/hello";

	// Information used when creating the native window.
	CefWindowInfo windowInfo;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	windowInfo.SetAsPopup(NULL, "Apoapse Pro");
#endif
	// Create the first browser window.
	CefBrowserHost::CreateBrowser(windowInfo, handler, url, browserSettings, nullptr);
}