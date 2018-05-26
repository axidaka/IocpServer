#include "stdafx.h"
#include "IOCPBase.h"

CIOCPBase::CIOCPBase(void)
	:m_IOCP(NULL)
{
}


CIOCPBase::~CIOCPBase(void)
{
}

BOOL CIOCPBase::CreateNewIOCP( DWORD NumberOfConcurrentThreads )
{
	m_IOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, NumberOfConcurrentThreads);

	return (m_IOCP != NULL);
}

BOOL CIOCPBase::ReleaseIOCP()
{
	if (m_IOCP != NULL)
	{
		CloseHandle(m_IOCP);
		m_IOCP = NULL;
	}
	return TRUE;
}

BOOL CIOCPBase::AssociateWithIOCP( HANDLE FileHandle, ULONG_PTR CompletionKey )
{
	assert(FileHandle != INVALID_HANDLE_VALUE);
	assert(m_IOCP != NULL);
	
	return (CreateIoCompletionPort(FileHandle, m_IOCP, CompletionKey, 0) == m_IOCP);
}
