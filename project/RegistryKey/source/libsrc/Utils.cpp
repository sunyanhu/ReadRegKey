#include "Utils.h"
namespace OCLS
{
const LPSTR GetLastErrorMessage(DWORD last_error)
{
    static char errmsg[512];

	if (!FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		last_error,
		0,
		errmsg,
		511,
		NULL))
	{
		/* if we fail, call ourself to find out why and return that error */
		return (GetLastErrorMessage(GetLastError()));
	}

	return(errmsg);
}
}
