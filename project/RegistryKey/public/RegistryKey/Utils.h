#ifndef __UTILS__
#define __UTILS__
#pragma warning(disable: 4201)   // nameless struct/union

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifndef _INC_TCHAR
#include <tchar.h>
#endif

#pragma warning(default: 4201)


///////////////////////////////////////////////////////////////////////////////
// Debugging defines...
///////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_ONLY
#ifdef _DEBUG
#define DEBUG_ONLY(x)   x
#else
#define DEBUG_ONLY(x)
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// Functions defined in this file...
///////////////////////////////////////////////////////////////////////////////
namespace OCLS
{
inline bool BOOL_to_bool(const BOOL bResult)
{
    // Convert a make believe BOOL into a real bool.
    // Removes warning C4800...

    return (TRUE == bResult);
}

const LPSTR GetLastErrorMessage(DWORD last_error);
}
#endif // __UTILS__

