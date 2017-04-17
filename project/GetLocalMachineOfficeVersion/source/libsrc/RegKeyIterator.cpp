#include "RegKeyIterator.h"

#pragma warning(disable: 4514)   // unreferenced inline function removed
#pragma warning(disable: 4710)   // function not expanded
namespace OCLS
{
CRegKeyIterator::CRegKeyIterator(CRegistryKeyCounted *pKey)
: m_pKey(CRegistryKeyCounted::Copy(pKey)),
m_index(0)
{
}

CRegKeyIterator::CRegKeyIterator(const CRegKeyIterator &rhs)
: m_pKey(CRegistryKeyCounted::Copy(rhs.m_pKey)),
m_index(rhs.m_index)
{
}

CRegKeyIterator::~CRegKeyIterator()
{
	if (m_pKey)
	{
		m_pKey->Release();
		m_pKey = 0;
	}
}

CRegKeyIterator &CRegKeyIterator::operator=(const CRegKeyIterator &rhs)
{
	if (this != &rhs)
	{
		CRegistryKeyCounted *pNewKey = CRegistryKeyCounted::Copy(rhs.m_pKey);
		m_pKey->Release();
		m_pKey = pNewKey;

		m_index = rhs.m_index;
	}

	return *this;
}

bool CRegKeyIterator::operator==(const CRegKeyIterator &rhs) const
{
	return (m_pKey == rhs.m_pKey && m_index == rhs.m_index);
}
}