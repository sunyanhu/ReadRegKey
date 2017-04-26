#include <Windows.h>

HINSTANCE g_VersionInstance = 0;
BOOL WINAPI DllMain(HINSTANCE hinstDLL,
	DWORD  dwReason,
	LPVOID lpvReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_VersionInstance = hinstDLL;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
