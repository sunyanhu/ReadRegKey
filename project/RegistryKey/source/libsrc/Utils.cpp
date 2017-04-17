#include "Utils.h"
namespace OCLS
{
const LPTSTR GetLastErrorMessage(DWORD last_error)
{
	static TCHAR errmsg[512];

	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
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
