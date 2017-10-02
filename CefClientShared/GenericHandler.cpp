#include "GenericHandler.h"

#include <sstream>
#include <string>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

namespace {

	GenericHandler* g_instance = nullptr;

}  // namespace

GenericHandler::GenericHandler() : m_isClosing(false)
{
	DCHECK(!g_instance);
	g_instance = this;
}

GenericHandler::~GenericHandler() {
	g_instance = nullptr;
}

// static
GenericHandler* GenericHandler::GetInstance() {
	return g_instance;
}

void GenericHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear();
}

bool GenericHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
	return true;
}

CefRequestHandler::ReturnValue GenericHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
	const std::string url = request->GetURL();
	static const std::string apoapseUrl = "http://apoapse/";

	if (url.substr(0, apoapseUrl.length()) == apoapseUrl)
	{
		return RV_CONTINUE;
	}
	else
	{
		DLOG(INFO) << "GenericHandler::OnBeforeResourceLoad Blocked the request: " << apoapseUrl;
		return RV_CANCEL;
	}
}

void GenericHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

}

void GenericHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Add to the list of existing browsers.
	m_browserList.push_back(browser);

	//browser->GetMainFrame()->ExecuteJavaScript("alert('yup');", browser->GetMainFrame()->GetURL(), 0);
}

bool GenericHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	if (m_browserList.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		m_isClosing = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void GenericHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers.

	BrowserList::iterator bit = m_browserList.begin();
	for (; bit != m_browserList.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			m_browserList.erase(bit);
			break;
		}
	}

	if (m_browserList.empty()) {
		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
}

void GenericHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL "
		<< std::string(failedUrl) << " with error " << std::string(errorText)
		<< " (" << errorCode << ").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void GenericHandler::CloseAllBrowsers(bool forceClose)
{
	if (!CefCurrentlyOn(TID_UI)) {
		CefPostTask(TID_UI, base::Bind(&GenericHandler::CloseAllBrowsers, this, forceClose));
		return;
	}

	if (m_browserList.empty())
		return;

	for (auto& browser : m_browserList)
		browser->GetHost()->CloseBrowser(forceClose);
}
