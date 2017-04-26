#include "Win32Exception.h"
#include "Utils.h"
namespace OCLS
{
CLSException::CLSException(
    const LPCSTR pWhere,
	DWORD error)
	: m_pWhere(pWhere),
	m_error(error)
{
	//   DEBUG_ONLY(MessageBox());
}

CLSException::CLSException()
{
	m_error = ::GetLastError();
}

DWORD CLSException::GetError() const
{
	return m_error;
}

LPCSTR CLSException::GetWhere() const
{
	// Wrap the message with the file and line?
	return m_pWhere;
}

LPCSTR CLSException::GetMessage() const
{
	return GetLastErrorMessage(m_error);
}

void CLSException::MessageBox(HWND hWnd /* = NULL */) const
{
	::MessageBoxA(hWnd, GetMessage(), m_pWhere, MB_ICONSTOP);
}
}