#include "RegistryKey.h"
#include <stdio.h>
#include <malloc.h>
#include <WinDNS.h>

namespace OCLS
{
CRegistryKey::CRegistryKey(
	HKEY hKey)
	: m_pKey(NewCountedKey(hKey, true))
{
}

CRegistryKey::CRegistryKey(
	LPTSTR pRemoteMachine,
	HKEY hKey)
	: m_pKey(0)
{
	HKEY theKey = hKey;

	LONG result = RegConnectRegistry(pRemoteMachine, hKey, &theKey);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::CRegistryKey() - RegConnectRegistry"), result);
	}

	m_pKey = NewCountedKey(theKey, true);
}


CRegistryKey::CRegistryKey(
	HKEY hKey,
	LPCTSTR pSubKey,
	REGSAM samDesired /* = KEY_ALL_ACCESS */,
	LPTSTR pRemoteMachine /* = 0 */)
	: m_pKey(0)
{
	HKEY theKey = hKey;

	if (pRemoteMachine)
	{
		LONG result = RegConnectRegistry(pRemoteMachine, hKey, &theKey);

		if (ERROR_SUCCESS != result)
		{
			throw CLSException(_T("CRegistryKey::CRegistryKey() - RegConnectRegistry"), result);
		}
	}

	HKEY newKey;
	LONG result = RegOpenKeyEx(theKey, pSubKey, 0, samDesired, &newKey);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::CRegistryKey(HKEY hKey ...)"), result);
	}

	m_pKey = NewCountedKey(newKey, true);
}

CRegistryKey::CRegistryKey(const CRegistryKey &rhs)
	: m_pKey(rhs.m_pKey->AddRef())
{

}

CRegistryKey::~CRegistryKey()
{
	m_pKey = m_pKey->Release();
}

///////////////////////////////////////////////////////////////////////////////
// Assignment
///////////////////////////////////////////////////////////////////////////////

CRegistryKey &CRegistryKey::operator=(const CRegistryKey &rhs)
{
	if (this != &rhs)
	{
		CRegistryKeyCounted *pNewKey = rhs.m_pKey->AddRef();

		m_pKey->Release();

		m_pKey = pNewKey;
	}

	return *this;
}

// NOTE: We OWN This hKey from this point on...

CRegistryKey &CRegistryKey::operator=(HKEY hKey)
{
	CRegistryKeyCounted *pNewKey = NewCountedKey(hKey, true);

	m_pKey->Release();

	m_pKey = pNewKey;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Static helper function...
///////////////////////////////////////////////////////////////////////////////

CRegistryKeyCounted *CRegistryKey::NewCountedKey(
	HKEY hKey,
	bool bCloseKeyOnFailure /* = false */)
{
	CRegistryKeyCounted *pCRegistryKeyCounted = 0;

	try
	{
		pCRegistryKeyCounted = new CRegistryKeyCounted(hKey);
	}
	catch (...) //xalloc &e)
	{
		pCRegistryKeyCounted = 0;
	}

	if (!pCRegistryKeyCounted)
	{
		if (bCloseKeyOnFailure)
		{
			RegCloseKey(hKey);
		}

		throw CLSException(_T("CRegistryKey::NewCRegistryKeyCounted()"),ERROR_NOT_ENOUGH_MEMORY);
	}

	return pCRegistryKeyCounted;
}

///////////////////////////////////////////////////////////////////////////////
// Registry API wrappers...
///////////////////////////////////////////////////////////////////////////////

CRegistryKey CRegistryKey::OpenKey(
	LPCTSTR pSubKey,
	REGSAM samDesired /* = KEY_ALL_ACCESS */) const
{
	return CRegistryKey(*this, pSubKey, samDesired);
}

void CRegistryKey::DeleteKey(
	LPCTSTR pKeyName) const
{
	// Behaviour of RegDeleteKey differs on Win95 and NT
	// On 95 RegDeleteKey will delete keys with subkeys
	// on NT it wont.
	// To add some consistency to the world DeleteKey 
	// will always fail to delete a key with sub keys and
	// DeleteKeyAndSubKeys will always work...


	// scope the key...
	{
		CRegistryKey deadKey = OpenKey(pKeyName);

		if (deadKey.BeginSubkeyIteration() != deadKey.EndSubkeyIteration())
		{
			throw CLSException(_T("CRegistryKey::DeleteKey()"), ERROR_ACCESS_DENIED);
		}
	}

	LONG result = RegDeleteKey(m_pKey->GetCounted(), pKeyName);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::DeleteKey()"), result);
	}
}

void CRegistryKey::DeleteKeyAndSubkeys(
	LPCTSTR pKeyName) const
{
	// Win95 doesn't need this as it deletes subkeys by default
	// NT wont delete a key with subkeys...

	// Scope the deadKey...
	{
		CRegistryKey deadKey = OpenKey(pKeyName);

		// This wont work, we are frigging with the sub keys of the key we're
		// iterating...

		//      for (SubkeyIterator it = deadKey.BeginSubkeyIteration();
		//         it != deadKey.EndSubkeyIteration();
		//        ++it)
		//      {
		//         deadKey.DeleteKeyAndSubkeys(it.GetName());
		//      }

		for (SubkeyIterator it = deadKey.BeginSubkeyIteration();
			it != deadKey.EndSubkeyIteration();
			it = deadKey.BeginSubkeyIteration())
		{
			deadKey.DeleteKeyAndSubkeys(it.GetName());
		}


	}

	DeleteKey(pKeyName);
}

CRegistryKey CRegistryKey::CreateKey(
	LPCTSTR pSubKey,
	LPTSTR pClass /* = _T("") */,
	DWORD dwOptions /* = REG_OPTION_NON_VOLATILE */,
	REGSAM samDesired /* = KEY_ALL_ACCESS */,
	LPSECURITY_ATTRIBUTES pSecurityAttributes /* = NULL */) const
{
	DWORD disposition;

	CRegistryKey key = CreateOrOpenKey(
		pSubKey,
		&disposition,
		pClass,
		dwOptions,
		samDesired,
		pSecurityAttributes);

	if (disposition != REG_CREATED_NEW_KEY)
	{
		RegCloseKey(key);

		throw CLSException(_T("CRegistryKey::CreateKey()"), ERROR_ALREADY_EXISTS);
	}


	return key;
}

CRegistryKey CRegistryKey::CreateOrOpenKey(
	LPCTSTR pSubKey,
	DWORD *pDisposition /* = NULL */,
	LPTSTR pClass /* = _T("") */,
	DWORD dwOptions /* = REG_OPTION_NON_VOLATILE */,
	REGSAM samDesired /* = KEY_ALL_ACCESS */,
	LPSECURITY_ATTRIBUTES pSecurityAttributes /* = NULL */) const
{
	HKEY hKey;

	DWORD disposition;

	if (!pDisposition)
	{
		pDisposition = &disposition;
	}

	LONG result = RegCreateKeyEx(m_pKey->GetCounted(), pSubKey, 0, pClass, dwOptions,
		samDesired, pSecurityAttributes, &hKey,
		pDisposition);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::CreateOrOpenKey()"), result);
	}

	return CRegistryKey(hKey);
}

bool CRegistryKey::HasSubkey(
	LPCTSTR pSubKey,
	REGSAM samDesired /* = KEY_ALL_ACCESS */) const
{
	bool hasKey = false;

	HKEY hKey;

	LONG result = RegOpenKeyEx(m_pKey->GetCounted(), pSubKey, 0, samDesired, &hKey);

	if (ERROR_SUCCESS == result)
	{
		hasKey = true;

		RegCloseKey(hKey);
	}
	else // should check for errors that mean no we dont have it, or we 
		// have it but not with the right access and throw if a real error
	{
		hasKey = false;
	}

	return hasKey;
}

CRegistryKey CRegistryKey::ConnectRegistry(LPTSTR pMachineName) const
{
	HKEY hKey;

	LONG result = RegConnectRegistry(pMachineName, m_pKey->GetCounted(), &hKey);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::ConnectRegistry()"), result);
	}

	return CRegistryKey(hKey);
}

void CRegistryKey::FlushKey() const
{
	LONG result = RegFlushKey(m_pKey->GetCounted());

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::FlushKey()"), result);
	}
}

CRegistryKey::operator HKEY() const
{
	return m_pKey->GetCounted();
}

void CRegistryKey::DeleteValue(LPCTSTR pValueName) const
{
	LONG result = RegDeleteValue(m_pKey->GetCounted(), pValueName);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::DeleteValue()"), result);
	}
}

// Subkey iteration

CRegistryKey::SubkeyIterator CRegistryKey::BeginSubkeyIteration() const
{
	return SubkeyIterator(m_pKey);
}

CRegistryKey::SubkeyIterator CRegistryKey::EndSubkeyIteration() const
{
	return SubkeyIterator(0);
}
///////////////////////////////////////////////////////////////////////////////
// Value iteration
///////////////////////////////////////////////////////////////////////////////

CRegistryKey::ValueIterator CRegistryKey::BeginValueIteration() const
{
	return ValueIterator(m_pKey);
}

CRegistryKey::ValueIterator CRegistryKey::EndValueIteration() const
{
	return ValueIterator(0);
}

///////////////////////////////////////////////////////////////////////////////
// Query values
///////////////////////////////////////////////////////////////////////////////
// QueryValue for other types
// What about multiple values?

CRegistryValue CRegistryKey::QueryValue(LPCTSTR pValueName /* = 0 */) const
{
	DWORD dwType;
	LPBYTE pBuffer = 0;
	DWORD bufSize = 0;

	LONG result = RegQueryValueEx(
		m_pKey->GetCounted(),
		pValueName,
		0,
		&dwType,
		pBuffer,
		&bufSize);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::QueryValue()"), result);
	}

	// bufSize should now tell us how much space we need...

	if (dwType == REG_DWORD)
	{
		pBuffer = (LPBYTE)malloc(sizeof(DWORD));
	}
	else
	{
		pBuffer = (unsigned char *)malloc(bufSize + 1);
		memset(pBuffer, '\0', bufSize + 1);
	}

	result = RegQueryValueEx(
		m_pKey->GetCounted(),
		pValueName,
		0,
		&dwType,
		pBuffer,
		&bufSize);

	if (ERROR_SUCCESS != result)
	{
		free(pBuffer); //crap

		throw CLSException(_T("CRegistryKey::QueryValue()"), result);
	}

	//lil
	CRegistryValue retValue(pValueName, pBuffer, bufSize, dwType);

	if (pBuffer != NULL)
		free(pBuffer);

	return retValue;
}

void CRegistryKey::SetValue(
	const CRegistryValue &value) const
{
	// Should the value store the name too?
	LONG result = RegSetValueEx(
		m_pKey->GetCounted(),
		value.m_pName,
		0,
		value.m_dwType,
		value.m_pBuffer,
		value.m_bufSize);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::SetValue()"), result);
	}
}

void CRegistryKey::LoadKey(LPCTSTR pSubkeyName, LPCTSTR pFile) const
{
	LONG result = RegLoadKey(m_pKey->GetCounted(), pSubkeyName, pFile);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::LoadKey()"), result);
	}
}

void CRegistryKey::UnLoadKey(LPCTSTR pSubkeyName) const
{
	LONG result = RegUnLoadKey(m_pKey->GetCounted(), pSubkeyName);

	if (ERROR_SUCCESS == result)
	{
		throw CLSException(_T("CRegistryKey::UnLoadKey()"), result);
	}
}

void CRegistryKey::SaveKey(
	LPCTSTR pFile,
	LPSECURITY_ATTRIBUTES pSecurityAttributes /* = NULL */) const
{
	LONG result = RegSaveKey(m_pKey->GetCounted(), pFile, pSecurityAttributes);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::SaveKey()"), result);
	}
}

void CRegistryKey::RestoreKey(LPCTSTR pFile, DWORD flags /* = 0 */) const
{
	LONG result = RegRestoreKey(m_pKey->GetCounted(), pFile, flags);

	if (ERROR_SUCCESS == result)
	{
		throw CLSException(_T("CRegistryKey::RestoreKey()"), result);
	}
}

void CRegistryKey::ReplaceKey(
	LPCTSTR pNewFile,
	LPCTSTR pOldFile,
	LPCTSTR pSubkeyName /* = 0 */) const
{
	LONG result = RegReplaceKey(m_pKey->GetCounted(), pSubkeyName, pNewFile, pOldFile);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::ReplaceKey()"), result);
	}
}

PSECURITY_DESCRIPTOR CRegistryKey::GetKeySecurity(
	SECURITY_INFORMATION securityInformation) const
{
	TExpandableBuffer<unsigned char> descriptor;

	DWORD dwSize = 0;

	LONG result = RegGetKeySecurity(m_pKey->GetCounted(),
		securityInformation,
		0,
		&dwSize);

	if (ERROR_INSUFFICIENT_BUFFER == result)
	{
		descriptor.Resize(dwSize);

		result = RegGetKeySecurity(m_pKey->GetCounted(),
			securityInformation,
			(PSECURITY_DESCRIPTOR)descriptor.GetBuffer(),
			&dwSize);
	}

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::GetKeySecurity()"), result);
	}

	return descriptor.ReleaseBuffer();
}


void CRegistryKey::SetKeySecurity(
	SECURITY_INFORMATION securityInformation,
	PSECURITY_DESCRIPTOR pSecurityDescriptor) const
{
	LONG result = RegSetKeySecurity(m_pKey->GetCounted(),
		securityInformation,
		pSecurityDescriptor);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::SetKeySecurity()"), result);
	}
}

void CRegistryKey::NotifyChangeKeyValue(
	HANDLE hEvent,
	bool bSubKeys /* = false */,
	DWORD dwNotifyFilter /* = REG_NOTIFY_CHANGE_LAST_SET */) const
{
	LONG result = RegNotifyChangeKeyValue(m_pKey->GetCounted(),
		bSubKeys,
		dwNotifyFilter,
		hEvent,
		true);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::RegNotifyChangeKeyValue()"), result);
	}
}

void CRegistryKey::NotifyChangeKeyValue(
	bool bSubKeys /* = false */,
	DWORD dwNotifyFilter /* = REG_NOTIFY_CHANGE_LAST_SET */) const
{
	LONG result = RegNotifyChangeKeyValue(m_pKey->GetCounted(),
		bSubKeys,
		dwNotifyFilter,
		0,
		false);

	if (ERROR_SUCCESS != result)
	{
		throw CLSException(_T("CRegistryKey::RegNotifyChangeKeyValue()"), result);
	}
}


///////////////////////////////////////////////////////////////////////////////
// CRegistryKey::SubkeyIterator
///////////////////////////////////////////////////////////////////////////////

CRegistrySubkeyIteratorImpl::CRegistrySubkeyIteratorImpl(CRegistryKeyCounted *pKey)
	: CRegKeyIterator(pKey),
	m_Name(0),
	m_Class(0)
{
	if (pKey)
	{
		DWORD dwNumSubKeys = 0;
		DWORD dwMaxNameLen = 0;
		DWORD dwMaxClassLen = 0;

		LONG result = RegQueryInfoKey(
			pKey->GetCounted(),
			NULL,    // Not interested in this key's class 
			NULL,    // ditto
			NULL,    // Reserved
			&dwNumSubKeys,
			&dwMaxNameLen,
			&dwMaxClassLen,
			NULL,    // Not interested in number of values 
			NULL,    // Not interested in max length of value name
			NULL,    // Not interested in max length of value buffer
			NULL,    // Not interested in length of security descriptor
			NULL);   // Not interested in last write time

		if (ERROR_SUCCESS != result)
		{
			throw CLSException(_T("CRegistryKey::SubkeyIterator::SubkeyIterator()"),result);
		}

		// wangyongli delete            
		//             if (0 != dwNumSubKeys)
		//             {
		// Allow for NULL character...

		m_Name.Resize(dwMaxNameLen + 1);
		m_Class.Resize(dwMaxClassLen + 1);
		/*            }*/
	}
}

bool CRegistrySubkeyIteratorImpl::operator==(const CRegistrySubkeyIteratorImpl &rhs) const
{
	return CRegKeyIterator::operator==(rhs);
}

LPCTSTR CRegistrySubkeyIteratorImpl::GetName() const
{
	return m_Name;
}

LPCTSTR CRegistrySubkeyIteratorImpl::GetClass() const
{
	return m_Class;
}

CRegistryKey CRegistrySubkeyIteratorImpl::OpenKey(
	REGSAM samDesired /*= KEY_ALL_ACCESS*/) const
{
	return CRegistryKey(m_pKey->GetCounted(), m_Name, samDesired);
}

bool CRegistrySubkeyIteratorImpl::GetItem()
{
	FILETIME m_lastWriteTime;
	DWORD baseGrow = 10;
	int level = 0;
	bool ok = true;
	bool done = false;

	// Could initialise with a call to GetKeyInfo to get longest key name etc

	while (!done && ok)
	{
		DWORD nameLen = m_Name.GetSize();
		DWORD classLen = m_Class.GetSize();

		LONG result = RegEnumKeyEx(
			m_pKey->GetCounted(),
			m_index,
			m_Name,
			&nameLen,
			NULL,
			m_Class,
			&classLen,
			&m_lastWriteTime);

		if (ERROR_NO_MORE_ITEMS == result)
		{
			ok = false;
		}
		else if (ERROR_MORE_DATA == result)
		{
			// Size has changed since we started 

			DWORD dwNumSubKeys = 0;
			DWORD dwMaxNameLen = 0;
			DWORD dwMaxClassLen = 0;

			result = RegQueryInfoKey(
				m_pKey->GetCounted(),
				NULL,    // Not interested in this key's class 
				NULL,    // ditto
				NULL,    // Reserved
				&dwNumSubKeys,
				&dwMaxNameLen,
				&dwMaxClassLen,
				NULL,    // Not interested in number of values 
				NULL,    // Not interested in max length of value name
				NULL,    // Not interested in max length of value buffer
				NULL,    // Not interested in length of security descriptor
				NULL);   // Not interested in last write time

			if (ERROR_SUCCESS != result)
			{
				throw CLSException(_T("CRegistryKey::SubkeyIterator::GetSubkeyInfo()"),result);
			}

			if (0 != dwNumSubKeys)
			{
				// Allow for NULL character...
				level++;
				if (dwMaxNameLen > nameLen - 1)
				{
					m_Name.Resize(dwMaxNameLen + 1);
				}
				else
				{
					m_Name.Resize(dwMaxNameLen + level*baseGrow + 1);
				}

				if (dwMaxClassLen > classLen - 1)
				{
					m_Class.Resize(dwMaxClassLen + 1);
				}
				else
				{
					m_Class.Resize(dwMaxClassLen + level*baseGrow + 1);
				}
			}
			else
			{
				done = true;
			}
		}
		else if (ERROR_SUCCESS != result)
		{
			throw CLSException(_T("CRegistryKey::SubkeyIterator::GetSubkeyInfo()"),result);
		}
		else
		{
			done = true;
		}
	}
	return ok;
}
///////////////////////////////////////////////////////////////////////////////
// CRegistryValueIteratorImpl
///////////////////////////////////////////////////////////////////////////////

CRegistryValueIteratorImpl::CRegistryValueIteratorImpl(CRegistryKeyCounted *pKey)
	: CRegKeyIterator(pKey),
	m_Name(0),
	m_Buffer(0),
	m_dwType(REG_NONE),
	m_dwBufUsed(0),
	m_pStringRep(0)
{
	if (m_pKey)
	{
		DWORD dwNumValues = 0;
		DWORD dwMaxValueNameLen = 0;
		DWORD dwMaxValueLen = 0;

		LONG result = RegQueryInfoKey(
			m_pKey->GetCounted(),
			NULL,    // Not interested in this key's class 
			NULL,    // ditto
			NULL,    // Reserved
			NULL,    // Not interested in number of sub keys
			NULL,    // Not interested in max sub key name length
			NULL,    // Not interested in max sub key class length
			&dwNumValues,
			&dwMaxValueNameLen,
			&dwMaxValueLen,
			NULL,    // Not interested in length of security descriptor
			NULL);   // Not interested in last write time

		if (ERROR_SUCCESS != result)
		{
			throw CLSException(_T("CRegistryValueIterator::ValueIterator()"), result);
		}

		// wangyongli delete
		/*            if (0 != dwNumValues)*/
		/*            {*/
		// Allow for NULL character...

		m_Name.Resize(dwMaxValueNameLen + 1);
		m_Buffer.Resize(dwMaxValueLen + 1);
		m_dwBufUsed = 0;
		m_dwType = REG_NONE;
		/*            }*/
		// wangyongli delete
	}
}

bool CRegistryValueIteratorImpl::operator==(
	const CRegistryValueIteratorImpl &rhs) const
{
	return CRegKeyIterator::operator==(rhs);
}

LPCTSTR CRegistryValueIteratorImpl::GetName() const
{
	return m_Name;
}

LPCTSTR CRegistryValueIteratorImpl::AsString() const
{
	if (!m_pStringRep)
	{
		m_pStringRep = CRegistryValue::AsString(m_dwType, m_Buffer, m_dwBufUsed);
	}

	return m_pStringRep;
}

CRegistryValueIteratorImpl::operator CRegistryValue() const
{
	return CRegistryValue(m_Name, m_Buffer, m_dwBufUsed, m_dwType);
}

bool CRegistryValueIteratorImpl::GetItem()
{
	bool ok = true;
	bool done = false;

	// String representation cache is no longer valid
	delete[] m_pStringRep;
	m_pStringRep = 0;

	while (!done && ok)
	{

		DWORD nameLen = m_Name.GetSize();
		m_dwBufUsed = m_Buffer.GetSize();

		LONG result = RegEnumValue(
			m_pKey->GetCounted(),
			m_index,
			m_Name,
			&nameLen,
			NULL,
			&m_dwType,
			m_Buffer,
			&m_dwBufUsed);

		if (ERROR_NO_MORE_ITEMS == result)
		{
			ok = false;
		}
		else if (ERROR_MORE_DATA == result)
		{
			DWORD dwNumValues = 0;
			DWORD dwMaxValueNameLen = 0;
			DWORD dwMaxValueLen = 0;

			result = RegQueryInfoKey(
				m_pKey->GetCounted(),
				NULL,    // Not interested in this key's class 
				NULL,    // ditto
				NULL,    // Reserved
				NULL,    // Not interested in number of sub keys
				NULL,    // Not interested in max sub key name length
				NULL,    // Not interested in max sub key class length
				&dwNumValues,
				&dwMaxValueNameLen,
				&dwMaxValueLen,
				NULL,    // Not interested in length of security descriptor
				NULL);   // Not interested in last write time

			if (ERROR_SUCCESS != result)
			{
				throw CLSException(_T("CRegistryValueIterator::GetValueInfo()"), result);
			}

			if (0 != dwNumValues)
			{
				// Allow for NULL character...

				m_Name.Resize(2 * dwMaxValueNameLen + 1);
				m_Buffer.Resize(2 * dwMaxValueLen + 1);
			}
			else
			{
				done = true;
			}
		}
		else if (ERROR_SUCCESS != result)
		{
			throw CLSException(_T("CRegistryValueIterator::GetValueInfo()"),result);
		}
		else
		{
			done = true;
		}
	}
	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// CRegistryValue
///////////////////////////////////////////////////////////////////////////////
CRegistryValue::CRegistryValue(
	LPCTSTR pName,
	const LPBYTE pBuffer,
	DWORD bufSize,
	DWORD dwType /*= REG_BINARY*/)
	: m_pName((LPTSTR)_tcsdup(pName)),
	m_dwType(dwType),
	m_pBuffer((LPBYTE)duplicateBuffer(pBuffer, bufSize)),
	m_bufSize(bufSize),
	m_pStringRep(0)
{
}

CRegistryValue::CRegistryValue(
	LPCTSTR pName,
	vector<string> & strArray,
	DWORD dwType /*= REG_MULTI_SZ*/)
	: m_pName((LPTSTR)_tcsdup(pName)),
	m_dwType(dwType),
	m_pStringRep(0),
	m_pBuffer(0),
	m_bufSize(0)
{
	for (size_t i = 0; i < strArray.size(); i++)
	{
		m_bufSize += strArray[i].length();
		m_bufSize += 1;
	}
	m_bufSize += 1;

	if (m_bufSize)
	{
		m_pBuffer = (unsigned char *)malloc(m_bufSize + 1);
		memset(m_pBuffer, '\0', m_bufSize);
	}

	LPBYTE pBuffer = m_pBuffer;
	for (size_t i = 0; i < strArray.size(); i++)
	{
		memcpy(pBuffer, (LPCTSTR)strArray[i].c_str(), strArray[i].length());
		pBuffer += strArray[i].length();
		pBuffer += 1;
	}
}

CRegistryValue::CRegistryValue(
	LPCTSTR pName,
	LPCTSTR pString,
	DWORD dwType /*= REG_SZ */)
	: m_pName((LPTSTR)_tcsdup(pName)),
	m_dwType(dwType),
	m_pBuffer((LPBYTE)_tcsdup(pString)),
	m_bufSize((_tcslen(pString) + 1) * sizeof(TCHAR)),
	m_pStringRep(0)
{
	if (m_dwType != REG_EXPAND_SZ &&
		m_dwType != REG_SZ)
	{
		throw CLSException();
	}
}

CRegistryValue::CRegistryValue(
	LPCTSTR pName,
	DWORD dwValue,
	DWORD dwType /*= REG_DWORD*/)
	: m_pName((LPTSTR)_tcsdup(pName)),
	m_dwType(dwType),
	m_pBuffer((unsigned char *)malloc(sizeof(DWORD))),
	m_bufSize(sizeof(DWORD)),
	m_pStringRep(0)
{
	if (dwType != REG_DWORD &&
		dwType != REG_DWORD_LITTLE_ENDIAN &&
		dwType != REG_DWORD_BIG_ENDIAN)
	{
		throw CLSException();
	}

	memcpy(m_pBuffer, &dwValue, sizeof(DWORD));
}

CRegistryValue::CRegistryValue(const CRegistryValue &rhs)
	: m_pName(_tcsdup(rhs.m_pName)),
	m_dwType(rhs.m_dwType),
	m_pBuffer(rhs.duplicateBuffer()),
	m_bufSize(rhs.m_bufSize),
	m_pStringRep(0)
{
	memcpy(m_pBuffer, rhs.m_pBuffer, m_bufSize);
}

CRegistryValue::~CRegistryValue()
{
	if (m_pName != NULL)
	{
		//delete[] m_pName;
		free(m_pName);
	}

	if (m_pBuffer != NULL)
	{
		//delete[] m_pBuffer;
		free(m_pBuffer);
	}

	if (m_pStringRep != NULL)
	{
		//delete[] m_pStringRep;
		free(m_pStringRep);
	}
}

CRegistryValue &CRegistryValue::operator=(const CRegistryValue &rhs)
{
	if (this != &rhs)
	{
		LPBYTE pNewBuffer = rhs.duplicateBuffer();
		LPBYTE pOldBuffer = m_pBuffer;

		LPTSTR pNewName = _tcsdup(rhs.m_pName);
		LPTSTR pOldName = m_pName;

		m_dwType = rhs.m_dwType;
		m_bufSize = rhs.m_bufSize;

		m_pBuffer = pNewBuffer;
		m_pName = pNewName;

		free(pOldBuffer);
		free(pOldName);

		free(m_pStringRep);
		m_pStringRep = 0;
	}

	return *this;
}

CRegistryValue::operator LPBYTE() const
{
	if (m_dwType != REG_BINARY &&
		m_dwType != REG_NONE)
	{
		throw CLSException();
	}

	return m_pBuffer;
}

CRegistryValue::operator LPCTSTR() const
{
	if (m_dwType != REG_EXPAND_SZ &&
		m_dwType != REG_SZ)
	{
		throw CLSException();
	}

	return (LPCTSTR)m_pBuffer;
}

CRegistryValue::operator DWORD() const
{
	if (m_dwType != REG_DWORD &&
		m_dwType != REG_DWORD_LITTLE_ENDIAN &&
		m_dwType != REG_DWORD_BIG_ENDIAN)
	{
		throw CLSException();
	}

	return *(DWORD*)m_pBuffer;
}

LPCTSTR CRegistryValue::Name() const
{
	return m_pName;
}

LPCTSTR CRegistryValue::AsString() const
{
	if (!m_pStringRep)
	{
		m_pStringRep = AsString(m_dwType, m_pBuffer, m_bufSize);
	}

	return m_pStringRep;
}

LPTSTR CRegistryValue::AsString(
	DWORD dwType,
	const LPBYTE pBuffer,
	DWORD bufSize)
{
	LPTSTR pStringRep = NULL;

	// TODO Convert all reps to a string rep

	if (dwType == REG_EXPAND_SZ ||
		dwType == REG_SZ)
	{
		// Already a string!

		pStringRep = _tcsdup((LPTSTR)pBuffer);
	}
	else if (dwType == REG_MULTI_SZ)
	{
		LPBYTE pTemp = (unsigned char *)malloc(bufSize + 1);
		memset(pTemp, '\0', bufSize + 1);

		// multiple strings...
		// Should strip the \0's out...
		if (bufSize >= 2)
		{
			//lil
			//LPBYTE pTemp = new unsigned char[bufSize];

			if (pTemp != NULL)
			{
				memcpy(pTemp, pBuffer, bufSize);
				BOOL bHead = FALSE;
				for (DWORD i = 0; i < bufSize - 2; i++)
				{
					if (pTemp[i] == '\0' && !bHead)
					{
						pTemp[i] = ' ';
						continue;
					}
					else{
						bHead = TRUE;
					}

					if (pTemp[i] == '\0' && i)
						pTemp[i] = ',';
					if (pTemp[i] == ' ' && i)
						pTemp[i] = ',';
				}
				pStringRep = _tcsdup((LPTSTR)pTemp);
				free(pTemp);
			}
		}
		else{
			pStringRep = _tcsdup((LPTSTR)pTemp);
			free(pTemp);
		}
	}
	else if (dwType == REG_BINARY ||
		dwType == REG_NONE)
	{
		pStringRep = bytesAsString(pBuffer, bufSize);
	}
	else if (dwType == REG_QWORD)
	{
		const int nQWStrLen = 30;
		union QwordValue_un
		{
			byte bBuf[8];
			QWORD qwValue;
		};
		QwordValue_un unDwordValue;
		unDwordValue.qwValue = 0;
		memcpy(unDwordValue.bBuf, pBuffer, 8);
		pStringRep = (char*)malloc(nQWStrLen);
		memset(pStringRep, 0, nQWStrLen);
		if (pStringRep)
		{
			sprintf_s(pStringRep, nQWStrLen, "%d", unDwordValue.qwValue);
		}
	}
	// wangyonli add
	else if (dwType == REG_DWORD)
	{
		// ×¢Òâ´óÐ¡
		//lil
		//pStringRep = new TCHAR[2];
		const int nDWStrLen = 30;
		union DwordValue_un
		{
			byte bBuf[4];
			DWORD dwValue;
		};
		DwordValue_un unDwordValue;
		unDwordValue.dwValue = 0;
		memcpy(unDwordValue.bBuf, pBuffer, 4);
		pStringRep = (char*)malloc(nDWStrLen);
		memset(pStringRep, 0, nDWStrLen);
		if (pStringRep)
		{
			sprintf_s(pStringRep, nDWStrLen, "%d", unDwordValue.dwValue);
		}
	}
	// wangyongli add
	else if (dwType == REG_DWORD_LITTLE_ENDIAN ||
		dwType == REG_DWORD_BIG_ENDIAN)
	{
		pStringRep = _tcsdup(_T("A number"));
	}
	else
	{
		// Anything else...
		pStringRep = bytesAsString(pBuffer, bufSize);
	}

	return pStringRep;
}

LPTSTR CRegistryValue::bytesAsString(
	const LPBYTE pBuffer,
	DWORD bufSize)
{
	//lil
	//LPTSTR pStringRep = new TCHAR[(bufSize * 2) + 1];
	LPTSTR pStringRep = (char *)malloc((bufSize * 2) + 1);
	LPTSTR pHere = pStringRep;
	LPBYTE pBuf = pBuffer;

	for (DWORD i = 0; i < bufSize; i++)
	{
		_stprintf_s(pHere, (bufSize * 2) + 1, _T("%2.2x"), *pBuf);

		pHere += 2;
		pBuf++;
	}

	return pStringRep;
}

LPBYTE CRegistryValue::duplicateBuffer() const
{
	return duplicateBuffer(m_pBuffer, m_bufSize);
}

LPBYTE CRegistryValue::duplicateBuffer(LPBYTE pBuffer, DWORD bufSize) const
{
	LPBYTE pNewBuffer = (unsigned char *)malloc(bufSize + 1);
	memset(pNewBuffer, '\0', bufSize + 1);
	memcpy(pNewBuffer, pBuffer, bufSize);

	return pNewBuffer;
}
}