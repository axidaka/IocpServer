#pragma once

#include "stldefine.h"
#include "ThreadPoolBase.h"
#include "IOCPBase.h"

/**
@brief    拥有IO完成端口的线程池
@function 
@author   zhqsong
@date	  2013/11/22
*/

class CThPoolWithIOCPBase
{
public:
	CThPoolWithIOCPBase(void);
	virtual ~CThPoolWithIOCPBase(void);

	void SetThreadPool(CThreadPoolBase* pThPoolBase)
	{
		assert(pThPoolBase != NULL);
		m_pThPool = std_shared_ptr<CThreadPoolBase>(pThPoolBase);
	}

	void SetIOCPBase(CIOCPBase* pIOCPBase)
	{
		assert(pIOCPBase != NULL);
		m_pIOCP = std_shared_ptr<CIOCPBase>(pIOCPBase);
	}

	RET_STATUS InitThPoolIOCP(int nThreads);
	RET_STATUS UninitThPoolIOCP();
	RET_STATUS BindHandleWithIOCP(HANDLE FileHandle, ULONG_PTR CompletionKey);

protected:

	std_shared_ptr<CThreadPoolBase> m_pThPool;
	std_shared_ptr<CIOCPBase>  m_pIOCP;
};

