#ifndef __EXPANDABLE_BUFFER__
#define __EXPANDABLE_BUFFER__
namespace OCLS
{
template <class T> class TExpandableBuffer;

template <class T> class TExpandableBuffer
{
public:

	TExpandableBuffer(size_t intialSize = 0);
	TExpandableBuffer(const TExpandableBuffer &rhs);

	~TExpandableBuffer();

	TExpandableBuffer &operator=(const TExpandableBuffer &rhs);

	// Access functions 

	T *GetBuffer() const;
	operator T *() const;

	size_t GetSize() const;

	// Make the buffer bigger 

	size_t Resize(size_t newSize);    // Destructive

	size_t Expand(size_t newSize);    // Non destructive

	T *ReleaseBuffer();

private:

	T *m_pBuffer;

	size_t m_size;
};

///////////////////////////////////////////////////////////////////////////////
// Construction and destruction
///////////////////////////////////////////////////////////////////////////////

template <class T>
TExpandableBuffer<T>::TExpandableBuffer(size_t initialSize)
: m_pBuffer(0), m_size(0)
{
	Resize(initialSize);
}

template <class T>
TExpandableBuffer<T>::TExpandableBuffer(const TExpandableBuffer<T> &rhs)
: m_pBuffer(0), m_size(0)
{
	Resize(rhs.m_size);

	for (size_t i = 0; i < m_size; i++)
	{
		m_pBuffer[i] = rhs.m_pBuffer[i];
	}
}

template <class T>
TExpandableBuffer<T>::~TExpandableBuffer()
{
	delete[] m_pBuffer;
}

template <class T>
TExpandableBuffer<T> &TExpandableBuffer<T>::operator=(const TExpandableBuffer<T> &rhs)
{
	if (this != &rhs)
	{
		if (m_size != rhs.m_size)
		{
			T *pNewBuffer = new T[rhs.m_size];

			delete[] m_pBuffer;

			m_pBuffer = pNewBuffer;

			m_size = rhs.m_size;
		}

		for (size_t i = 0; i < m_size; i++)
		{
			m_pBuffer[i] = rhs.m_pBuffer[i];
		}
	}

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// Access functions
///////////////////////////////////////////////////////////////////////////////

template <class T>
T *TExpandableBuffer<T>::GetBuffer() const
{
	return m_pBuffer;
}

template <class T>
TExpandableBuffer<T>::operator T *() const
{
	return GetBuffer();
}

template <class T>
size_t TExpandableBuffer<T>::GetSize() const
{
	return m_size;
}

template <class T>
T *TExpandableBuffer<T>::ReleaseBuffer()
{
	T *pBuffer = m_pBuffer;

	m_pBuffer = 0;
	m_size = 0;

	return pBuffer;
}

///////////////////////////////////////////////////////////////////////////////
// Make the buffer bigger
///////////////////////////////////////////////////////////////////////////////

template <class T>
size_t TExpandableBuffer<T>::Resize(size_t newSize)
{
	if (m_size < newSize)
	{
		T *pNewBuffer = new T[newSize];

		delete[] m_pBuffer;

		m_pBuffer = pNewBuffer;

		m_size = newSize;
	}

	return m_size;
}

template <class T>
size_t TExpandableBuffer<T>::Expand(size_t newSize)
{
	if (m_size < newSize)
	{
		T *pNewBuffer = new T[newSize];

		for (size_t i = 0; i < newSize; i++)
		{
			pNewBuffer[i] = m_pBuffer[i];
		}

		delete[] m_pBuffer;

		m_pBuffer = pNewBuffer;

		m_size = newSize;
	}

	return m_size;
}
}

#endif // __EXPANDABLE_BUFFER__

