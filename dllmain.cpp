#include "pch.h"
#include "Aeyth8/Proxy/Proxy.hpp"




int __stdcall DllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved) {
	DisableThreadLibraryCalls(hModule);

	if (ulReasonForCall != DLL_PROCESS_ATTACH)
		return 1;

	Proxy::Attach(hModule);

	return 1;
}

