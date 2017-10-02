#include "ApoapseCefApp.h"
#include "GenericHandler.h"

#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/views/cef_browser_view.h>
#include <include/views/cef_window.h>
#include <include/wrapper/cef_helpers.h>

#include "ApoapseSchemeHandler.h"
#include "ClientEntryPoint.h"

ApoapseCefApp::ApoapseCefApp() = default;

void ApoapseCefApp::OnBeforeCommandLineProcessing(const CefString& processType, CefRefPtr<CefCommandLine> commandLine)
{
	//commandLine->AppendSwitchWithValue("url", "about:credits");
	//commandLine->AppendSwitchWithValue("default-background-color", "ff0000ff");

}

void ApoapseCefApp::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();

	CefRegisterSchemeHandlerFactory("http", "apoapse", new ApoapseSchemeHandlerFactory);


	const bool useView = false;
	CefRefPtr<GenericHandler> handler(new GenericHandler);

	CefBrowserSettings browserSettings;

	const std::string url = "http://apoapse/resources/main.html";

	// Information used when creating the native window.
	CefWindowInfo windowInfo;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	windowInfo.SetAsPopup(NULL, "Apoapse Pro");
#endif
	// Create the first browser window.
	m_browser = CefBrowserHost::CreateBrowserSync(windowInfo, handler, url, browserSettings, nullptr);

	ApoapseClientEntry::RegisterSignalSender(this);
}

void ApoapseCefApp::SendSignal(const std::string& name, const std::string& data)
{
	if (!data.empty())
	{
		m_browser->GetMainFrame()->ExecuteJavaScript("$(document).trigger(\"" + name + "\", [\'" + data + "\']);", "", 1);
	}
	else
	{
		m_browser->GetMainFrame()->ExecuteJavaScript("$(document).trigger(\"" + name + "\");", "", 1);
	}
}