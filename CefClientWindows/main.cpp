#include "ClientEntryPoint.h"
#include <Windows.h>
#include <memory>

#include <include/cef_sandbox_win.h>
#include "ApoapseCefApp.h"
#include "Resource.h"

int main(int argcount, char* argv[])
{
	std::vector<std::string> launchArgs(argv + 1, argv + argcount);	// on Windows, the first argument is the path of the executable so we get rid of it

	CefEnableHighDPISupport();
	void* sandbox_info = nullptr;

#if defined(CEF_USE_SANDBOX)
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	CefMainArgs mainArgs;

	int exit_code = CefExecuteProcess(mainArgs, nullptr, sandbox_info);
	if (exit_code >= 0)
		return exit_code;

	CefSettings settings;
	settings.multi_threaded_message_loop = false;
	settings.command_line_args_disabled = true;
	settings.persist_session_cookies = 0;
	settings.log_severity = LOGSEVERITY_ERROR;

#ifdef DEBUG
	//settings.single_process = true;
	settings.remote_debugging_port = 8080;
#endif // DEBUG


#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	// Initialize Apoapse client (ApoapseClientShared.dll)
	if (ApoapseClientEntry::ClientMain(launchArgs) <= 0)
		return 0;

	// Initialize CEF
	CefRefPtr<ApoapseCefApp> app(new ApoapseCefApp);

	CefInitialize(mainArgs, settings, app.get(), sandbox_info);

	// Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
	CefRunMessageLoop();

	CefShutdown();
	ApoapseClientEntry::Shutdown();

	return 0;
}
