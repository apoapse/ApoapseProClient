//#include "Common.h"
#include "ClientEntryPoint.h"
// #include <iostream>
#include <Windows.h>

#include <include/cef_sandbox_win.h>
#include "ApoapseCefApp.h"

int main(int argcount, char* argv[])
{
	CefEnableHighDPISupport();
	void* sandbox_info = nullptr;

#if defined(CEF_USE_SANDBOX)
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	//CefMainArgs main_args(hInstance);
	CefMainArgs main_args;

	int exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
	if (exit_code >= 0)
		return exit_code;

	CefSettings settings;
#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	// Initialize CEF.
	CefRefPtr<ApoapseCefApp> app(new ApoapseCefApp);

	CefInitialize(main_args, settings, app.get(), sandbox_info);

	if (ClientMain(argcount, argv) <= 0)
		return 0;

	// Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
	CefRunMessageLoop();

	CefShutdown();

	return 0;
}
