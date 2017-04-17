#ifndef __REG_KEY_ITERATOR__
#define __REG_KEY_ITERATOR__
#include "CountedRegKey.h"
#include "Export.h"

namespace OCLS
{
template <class B> class TRegKeyIterator;

class CRegKeyIterator;
class CRegistryKey;
class CRegistryKeyCounted;

class OCLS_REGISTRYKEY_API CRegKeyIterator
{
public:

	virtual ~CRegKeyIterator();

protected:

	CRegKeyIterator(CRegistryKeyCounted *pKey);

	CRegKeyIterator(const CRegKeyIterator &rhs);

	CRegKeyIterator &operator=(const CRegKeyIterator &rhs);

	bool operator==(const CRegKeyIterator &rhs) const;

	CRegistryKeyCounted *m_pKey;

	DWORD m_index;
};


///////////////////////////////////////////////////////////////////////////////
// TRegKeyIterator
///////////////////////////////////////////////////////////////////////////////

template <class B>
class TRegKeyIterator : public B
{
public:

	friend class CRegistryKey;

	TRegKeyIterator<B> &operator++();
	TRegKeyIterator<B> operator++(int);

	bool operator!=(const TRegKeyIterator<B> &rhs) const;
	bool operator==(const TRegKeyIterator<B> &rhs) const;

private:

	TRegKeyIterator(CRegistryKeyCounted *pKey);

	void Advance();
};

///////////////////////////////////////////////////////////////////////////////
// Construction and destruction
///////////////////////////////////////////////////////////////////////////////

template <class B>
TRegKeyIterator<B>::TRegKeyIterator(CRegistryKeyCounted *pKey)
: B(pKey)
{
	Advance();
}

///////////////////////////////////////////////////////////////////////////////
// Iteration
///////////////////////////////////////////////////////////////////////////////

template <class B>
TRegKeyIterator<B> &TRegKeyIterator<B>::operator++()
{
	Advance();

	return *this;
}

template <class B>
TRegKeyIterator<B> TRegKeyIterator<B>::operator++(int)
{
	TRegKeyIterator<B> oldValue = *this;

	Advance();

	return oldValue;
}

///////////////////////////////////////////////////////////////////////////////
// Equality
///////////////////////////////////////////////////////////////////////////////

template <class B>
bool TRegKeyIterator<B>::operator!=(
	const TRegKeyIterator<B> &rhs) const
{
	// defer to operator==
	return !(*this == rhs);
}

template <class B>
bool TRegKeyIterator<B>::operator==(
	const TRegKeyIterator<B> &rhs) const
{
	return B::operator==(rhs);
}

///////////////////////////////////////////////////////////////////////////////
// Iteration helper function
///////////////////////////////////////////////////////////////////////////////

template <class B>
void TRegKeyIterator<B>::Advance()
{
	if (m_pKey)
	{
		if (!B::GetItem())
		{
			// End of the line

			m_pKey = m_pKey->Release();

			m_index = 0;
		}
		else
		{
			m_index++;
		}
	}
}
}

#endif // __REG_KEY_ITERATOR__

