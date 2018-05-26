#include "stdafx.h"
#include "Async.h "
#include <windows.h>

IAsyncIO::IAsyncIO(void)
	:m_lRefCount(1)
{
}


IAsyncIO::~IAsyncIO(void)
{
}

void IAsyncIO::AddRef()
{
	InterlockedIncrement(&m_lRefCount);
}

void IAsyncIO::Release()
{
	long lRefCount = InterlockedDecrement(&m_lRefCount);
	if (0 == lRefCount)
	{
		delete this;
		return ;
	}
}
