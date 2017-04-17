#ifndef __WIN32_EXCEPTION__
#define __WIN32_EXCEPTION__

#pragma warning(disable: 4201)  

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifndef _INC_TCHAR
#include <tchar.h>
#endif

#pragma warning(disable: 4201)


class CLSException
{
public:
	CLSException();

	CLSException(
		const LPCTSTR pWhere,
		DWORD error);

	DWORD GetError() const;

	LPCTSTR GetWhere() const;

	virtual LPCTSTR GetMessage() const;

	void MessageBox(HWND hWnd = NULL) const;

private:

	LPCTSTR m_pWhere;
	DWORD m_error;
};

#endif // __WIN32_EXCEPTION__
