#include <windows.h>
#include <tchar.h>
#include "winSystem\WinSystem.h"

#ifdef _DEBUG
#pragma comment(lib, "osald.lib")
#else
#pragma comment(lib, "osal.lib")
#endif

typedef void (WINAPI *PGetNativeSystemInfo)(LPSYSTEM_INFO);

namespace OCLS
{
CWinSystem::CWinSystem()
{
}

CWinSystem::~CWinSystem()
{
}

BOOL CWinSystem::GetCurrentVersionByFile(DWORD &majorVersion, DWORD &minorVersion)
{
    BOOL retCode = FALSE;
    //todo: 获取%windir%\\system32\\ntdll.dll文件的主版本和次版本号，用以获取操作系统的版本

    return retCode;
}

OPER_SYSTEM_TYPE CWinSystem::GetVersion()
{
    OPER_SYSTEM_TYPE enType = OS_UNKNOWN;

    DWORD majorVersion = 0;
    DWORD minorVersion = 0;

    if (GetCurrentVersionByFile(majorVersion, minorVersion))
    {
        OSVERSIONINFOEX osvi;
        osvi.dwOSVersionInfoSize = sizeof(osvi);

        if (GetVersionEx((LPOSVERSIONINFO)&osvi))
        {
            if (majorVersion == 10 && minorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION)
                enType = OS_WIN10;

            if (majorVersion == 6 && minorVersion == 3 && osvi.wProductType == VER_NT_WORKSTATION)
                enType = OS_WIN8;

            if (majorVersion == 6 && minorVersion == 2 && osvi.wProductType == VER_NT_WORKSTATION)
                enType = OS_WIN8;

            if (majorVersion == 6 && minorVersion == 3 && osvi.wProductType == VER_NT_SERVER)
                enType = OS_WIN2012;

            if (majorVersion == 6 && minorVersion == 2 && osvi.wProductType == VER_NT_SERVER)
                enType = OS_WIN2012;

            if (majorVersion == 6 && minorVersion == 1 && osvi.wProductType == VER_NT_WORKSTATION)
                enType = OS_WIN7;

            if (majorVersion == 6 && minorVersion == 1 && osvi.wProductType == VER_NT_SERVER)
                enType = OS_WIN2008;

            if (majorVersion == 6 && minorVersion == 0 && osvi.wProductType == VER_NT_SERVER)
                enType = OS_WIN2008;

            if (majorVersion == 6 && minorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION)
                enType = OS_VISTA;

            if (majorVersion == 5 && minorVersion == 2)
                enType = OS_WIN2003;

            if (majorVersion == 5 && minorVersion == 1)
                enType = OS_WINXP;

            if (majorVersion == 5 && minorVersion == 0)
                enType = OS_WIN2000;

            if (majorVersion == 4 && minorVersion == 90)
                enType = OS_WINDOWS_ME;

            if (majorVersion == 4 && minorVersion == 10)
                enType = OS_WINDOWS_NT;

            if (majorVersion == 4 && minorVersion == 0)
                enType = OS_WIN98;
        }
    }

    return enType;
}

BOOL CWinSystem::IsVersionZone(OPER_SYSTEM_TYPE startVer, OPER_SYSTEM_TYPE endVer)
{
    BOOL bResult = FALSE;
    OPER_SYSTEM_TYPE enType = GetVersion();

    if (enType >= startVer && enType <= endVer)
    {
        bResult = TRUE;
    }

    return bResult;
}

BOOL CWinSystem::IsVersionLater(OPER_SYSTEM_TYPE osVer)
{
    BOOL bResult = FALSE;
    OPER_SYSTEM_TYPE enType = GetVersion();

    if (enType >= osVer)
    {
        bResult = TRUE;
    }

    return bResult;
}

BOOL CWinSystem::IsVersion(OPER_SYSTEM_TYPE osVer)
{
    BOOL bResult = FALSE;
    OPER_SYSTEM_TYPE enType = GetVersion();

    if (enType == osVer)
    {
        bResult = TRUE;
    }

    return bResult;
}

BOOL CWinSystem::IsHomeEdition()
{
    BOOL bResult = FALSE;

    OSVERSIONINFOEX osvi;
    osvi.dwOSVersionInfoSize = sizeof(osvi);

    if (GetVersionEx((LPOSVERSIONINFO)&osvi))
    {
        if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
            bResult = TRUE;
    }

    return bResult;
}

BOOL CWinSystem::IsX64Platform()
{
    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(SYSTEM_INFO));

    PGetNativeSystemInfo pGetNativeSystemInfo = (PGetNativeSystemInfo)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetNativeSystemInfo");
    if (NULL != pGetNativeSystemInfo)
    {
        pGetNativeSystemInfo(&si);
    }
    else
    {
        GetSystemInfo(&si);
    }

    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    {
        return TRUE;
    }
    return FALSE;
}
}