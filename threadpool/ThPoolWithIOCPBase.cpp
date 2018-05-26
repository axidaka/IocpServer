#include "stdafx.h"
#include "ThPoolWithIOCPBase.h"


CThPoolWithIOCPBase::CThPoolWithIOCPBase(void)
	:m_pThPool(NULL)
	,m_pIOCP(NULL)
{
}


CThPoolWithIOCPBase::~CThPoolWithIOCPBase(void)
{
}

RET_STATUS CThPoolWithIOCPBase::InitThPoolIOCP( int nThreads )
{
	if (m_pThPool.get() == NULL || m_pIOCP.get() == NULL)
	{
		//TODO
	}

	m_pThPool->CreateThreadPool(nThreads);
	m_pIOCP->CreateNewIOCP(nThreads);

	return ERR_32CODE_SUCCESS;
}

RET_STATUS CThPoolWithIOCPBase::UninitThPoolIOCP()
{
	if (m_pThPool.get() == NULL || m_pIOCP.get() == NULL)
	{
		//TODO
	}

	m_pThPool->DestroyThreadPool();
	m_pIOCP->ReleaseIOCP();

	return ERR_32CODE_SUCCESS;
}

RET_STATUS CThPoolWithIOCPBase::BindHandleWithIOCP( HANDLE FileHandle, ULONG_PTR CompletionKey )
{
	if (m_pIOCP.get() != NULL)
	{
		//TODO
	}

	return m_pIOCP->AssociateWithIOCP(FileHandle, CompletionKey);
}
