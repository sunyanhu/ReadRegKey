#include "Win32Exception.h"
#include "Utils.h"
namespace OCLS
{
CLSException::CLSException(
	const LPCTSTR pWhere,
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

LPCTSTR CLSException::GetWhere() const
{
	// Wrap the message with the file and line?
	return m_pWhere;
}

LPCTSTR CLSException::GetMessage() const
{
	return GetLastErrorMessage(m_error);
}

void CLSException::MessageBox(HWND hWnd /* = NULL */) const
{
	::MessageBox(hWnd, GetMessage(), m_pWhere, MB_ICONSTOP);
}
}