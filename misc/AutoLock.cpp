#include "StdAfx.h"
#include "AutoLock.h"

CAutoCriticalSection::CAutoCriticalSection()
{
	InitializeCriticalSection(&m_cs);
}

CAutoCriticalSection::~CAutoCriticalSection()
{
	DeleteCriticalSection(&m_cs);
}

void CAutoCriticalSection::Lock()
{
	EnterCriticalSection(&m_cs);
}

void CAutoCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_cs);
}

////////////////////////////////////////////////////////////////////////////////
CAutoLock::CAutoLock( CAutoCriticalSection &lock )
	:m_autoLock(lock)
{
	m_autoLock.Lock();
}


CAutoLock::~CAutoLock(void)
{
	m_autoLock.Unlock();
}

