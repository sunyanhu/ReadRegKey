#ifndef __COUNTED_REG_KEY__
#define __COUNTED_REG_KEY__
#include <windows.h>
#include "ReferenceCounted.h"

namespace OCLS
{
///////////////////////////////////////////////////////////////////////////////
// Classes defined in this file...
///////////////////////////////////////////////////////////////////////////////
class CRegistryKeyCounted;

///////////////////////////////////////////////////////////////////////////////
// CRegistryKeyCounted
///////////////////////////////////////////////////////////////////////////////
class CRegistryKeyCounted : public TReferenceCounted<HKEY, CRegistryKeyCounted>
{
public:

	explicit CRegistryKeyCounted(const HKEY hKey)
		: TReferenceCounted<HKEY, CRegistryKeyCounted>(hKey)
	{
	}

protected:
	virtual void PreRelease()
	{
		RegCloseKey(m_counted);
	}

	~CRegistryKeyCounted()
	{
	}

private:
	// Restrict copying... Do not implement...
	CRegistryKeyCounted(const CRegistryKeyCounted &rhs);
	CRegistryKeyCounted &operator=(const CRegistryKeyCounted &rhs);
};
}

#endif // __COUNTED_REG_KEY__

