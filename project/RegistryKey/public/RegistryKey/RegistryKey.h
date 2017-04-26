#ifndef _OCLS_REGISTRYKEY_H_
#define _OCLS_REGISTRYKEY_H_
#include <windows.h>
#include "ReferenceCounted.h"
#include "RegKeyIterator.h"
#include "ExpandableBuffer.h"
#include "Win32Exception.h"
#include <vector>
#include <tchar.h>
#include <string>

using namespace std;
namespace OCLS
{
typedef TExpandableBuffer<char>    TCHAR_Buffer;
typedef TExpandableBuffer<BYTE>     BYTE_Buffer;

///////////////////////////////////////////////////////////////////////////////
// SubkeyIteratorImpl
///////////////////////////////////////////////////////////////////////////////
class OCLS_REGISTRYKEY_API CRegistrySubkeyIteratorImpl : public CRegKeyIterator
{
public:

	CRegistryKey OpenKey(REGSAM samDesired = KEY_ALL_ACCESS) const;
    LPCSTR GetName() const;
    LPCSTR GetClass() const;

protected:
	CRegistrySubkeyIteratorImpl(CRegistryKeyCounted *pKey);
	bool GetItem();
	bool operator==(const CRegistrySubkeyIteratorImpl &rhs) const;

private:
	TCHAR_Buffer m_Name;
	TCHAR_Buffer m_Class;
};

class CRegistryValueIteratorImpl;
///////////////////////////////////////////////////////////////////////////////
// CRegistryValue
///////////////////////////////////////////////////////////////////////////////
class OCLS_REGISTRYKEY_API CRegistryValue
{
public:
	friend CRegistryKey;
	friend CRegistryValueIteratorImpl;

	CRegistryValue(const CRegistryValue &rhs);
	~CRegistryValue();

	CRegistryValue &operator=(const CRegistryValue &rhs);
	operator LPBYTE() const;
    operator LPCSTR() const;
	operator DWORD() const;

	// other operators
    LPCSTR Name() const;
    LPCSTR AsString() const;

	CRegistryValue(
        LPCSTR pName,
		const LPBYTE pBuffer,
		DWORD bufSize,
		DWORD dwType = REG_BINARY);

	CRegistryValue(
        LPCSTR pName,
		vector<string> & strArray,
		DWORD dwType = REG_MULTI_SZ);

	CRegistryValue(
        LPCSTR pName,
        LPCSTR pString,
		DWORD dwType = REG_SZ);

	CRegistryValue(
        LPCSTR pName,
		DWORD dwValue,
		DWORD dwType = REG_DWORD);

private:
	static LPSTR AsString(
		DWORD dwType,
		const LPBYTE pBuffer,
		DWORD bufSize);

    static LPSTR bytesAsString(
		const LPBYTE pBuffer,
		DWORD bufSize);

	LPBYTE duplicateBuffer() const;

	LPBYTE duplicateBuffer(
		LPBYTE pBuffer,
		DWORD bufSize) const;
public:
    LPSTR          m_pName;
	DWORD           m_dwType;
	LPBYTE          m_pBuffer;
	DWORD           m_bufSize;
    mutable LPSTR  m_pStringRep;
};

///////////////////////////////////////////////////////////////////////////////
// CRegistryValueIteratorImpl
///////////////////////////////////////////////////////////////////////////////
class OCLS_REGISTRYKEY_API CRegistryValueIteratorImpl : public CRegKeyIterator
{
public:
    LPCSTR GetName() const;
    LPCSTR AsString() const;
	operator CRegistryValue() const;

protected:
	CRegistryValueIteratorImpl(CRegistryKeyCounted *pKey);
	bool GetItem();
	bool operator==(const CRegistryValueIteratorImpl &rhs) const;

private:
	TCHAR_Buffer m_Name;
	BYTE_Buffer m_Buffer;
	DWORD m_dwType;
	DWORD m_dwBufUsed;
    mutable LPSTR m_pStringRep;
};
///////////////////////////////////////////////////////////////////////////////
// CRegistryKey
///////////////////////////////////////////////////////////////////////////////
class OCLS_REGISTRYKEY_API CRegistryKey
{
public:
	// Easier to manage typedefs for our iterators
	typedef TRegKeyIterator<CRegistrySubkeyIteratorImpl> SubkeyIterator;
	typedef TRegKeyIterator<CRegistryValueIteratorImpl> ValueIterator;

	// Construction and destruction
	explicit CRegistryKey(
		HKEY hKey);

	CRegistryKey(
        LPSTR pRemoteMachine,
		HKEY hKey);

	CRegistryKey(HKEY hKey,
        LPCSTR pSubKey,
		REGSAM samDesired = KEY_ALL_ACCESS,
        LPSTR pRemoteMachine = 0);

	CRegistryKey(const CRegistryKey &rhs);

	~CRegistryKey();

	// Assignment

	CRegistryKey &operator=(const CRegistryKey &rhs);
	CRegistryKey &operator=(HKEY hKey);

	// Registry API

    CRegistryKey OpenKey(LPCSTR pSubKey,
		REGSAM samDesired = KEY_ALL_ACCESS) const;

	CRegistryKey CreateKey(
        LPCSTR pSubKey,
        LPSTR pClass = "",
		DWORD dwOptions = REG_OPTION_NON_VOLATILE,
		REGSAM samDesired = KEY_ALL_ACCESS,
		LPSECURITY_ATTRIBUTES pSecurityAttributes = NULL) const;

	CRegistryKey CreateOrOpenKey(
        LPCSTR pSubKey,
		DWORD *pDisposition = NULL,
        LPSTR pClass = "",
		DWORD dwOptions = REG_OPTION_NON_VOLATILE,
		REGSAM samDesired = KEY_ALL_ACCESS,
		LPSECURITY_ATTRIBUTES pSecurityAttributes = NULL) const;

	void DeleteKey(LPCSTR pKeyName) const;

    void DeleteKeyAndSubkeys(LPCSTR pKeyName) const;

	bool HasSubkey(
        LPCSTR pSubkey,
		REGSAM samDesired = KEY_ALL_ACCESS) const;

    CRegistryKey ConnectRegistry(LPSTR pMachineName) const;

	void FlushKey() const;

	operator HKEY() const;

	// Subkeys

	SubkeyIterator BeginSubkeyIteration() const;
	SubkeyIterator EndSubkeyIteration() const;

	// Values...

	void DeleteValue(
        LPCSTR pValueName) const;

	CRegistryValue QueryValue(
        LPCSTR pValueName = 0) const;

	void SetValue(
		const CRegistryValue &value) const;

	ValueIterator BeginValueIteration() const;
	ValueIterator EndValueIteration() const;

	void LoadKey(
        LPCSTR pSubkeyName,
        LPCSTR pFile) const;

	void UnLoadKey(
        LPCSTR pSubkeyName) const;

	void SaveKey(
        LPCSTR pFile,
		LPSECURITY_ATTRIBUTES pSecurityAttributes = NULL) const;

	void RestoreKey(
        LPCSTR pFile,
		DWORD flags = 0) const;

	void ReplaceKey(
        LPCSTR pNewFile,
        LPCSTR pOldFile,
        LPCSTR pSubkeyName = 0) const;

	PSECURITY_DESCRIPTOR GetKeySecurity(
		SECURITY_INFORMATION securityInformation) const;

	void SetKeySecurity(
		SECURITY_INFORMATION securityInformation,
		PSECURITY_DESCRIPTOR pSecurityDescriptor) const;

	void NotifyChangeKeyValue(
		HANDLE hEvent,
		bool bSubKeys = false,
		DWORD dwNotifyFilter = REG_NOTIFY_CHANGE_LAST_SET) const;

	void NotifyChangeKeyValue(
		bool bSubKeys = false,
		DWORD dwNotifyFilter = REG_NOTIFY_CHANGE_LAST_SET) const;

private:
	static CRegistryKeyCounted *NewCountedKey(
		HKEY hKey,
		bool bCloseKeyOnFailure = false);
	CRegistryKeyCounted *m_pKey;
};
}
#endif