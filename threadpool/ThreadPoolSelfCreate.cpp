#include "stdafx.h"
#include "ThreadPoolSelfCreate.h"

// CThreadPoolSelfCreate::CThreadPoolSelfCreate(void)
// 	:CThreadPoolBase()
// {
// 
// }

CThreadPoolSelfCreate::CThreadPoolSelfCreate( std::shared_ptr<CIOCPBase> pIOCPBase )
	:CThreadPoolBase()
	,m_pIOCP(pIOCPBase)
	,m_hQuitEvent(NULL)
{

}

CThreadPoolSelfCreate::~CThreadPoolSelfCreate(void)
{

}

RET_STATUS CThreadPoolSelfCreate::CreateThreadPool( int nThreads )
{
	SYSTEM_INFO sysInfo = {0};
	GetSystemInfo(&sysInfo);

	if (m_pIOCP.get() != NULL)
	{
		m_pIOCP->CreateNewIOCP(nThreads);
	}
	
	///< �����ֶ� ��ʼ���źŵ��¼����
	m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hQuitEvent == NULL)
	{
		return ERR_64CODE_SYSTEM_API(GetLastError());
	}

	nThreads = sysInfo.dwNumberOfProcessors* 2/* > MAX_THREAD_NUM ? sysInfo.dwNumberOfProcessors*2 : MAX_THREAD_NUM*/;

	for (int i = 0; i < nThreads; ++ i)
	{
		try
		{
			HANDLE hThread = CreateThread(NULL, 0, WorkThread, this, 0, NULL);
			if (hThread == NULL)
			{
				///<TODO �������ɹ��������ͷ��Ѿ���Ч��hThread
			}

			m_vcthThread.push_back(hThread);
		}
		catch (...)
		{
			
		}
	
	}

	return 0;
}

RET_STATUS CThreadPoolSelfCreate::DestroyThreadPool()
{
	///< GetQueuedCompletionStatusֻ�еõ����ѲŻ᷵�أ�������Ҫ�����Զ����ʶ
	int nThreads = m_vcthThread.size();
	for (int i = 0; i < nThreads; ++ i)
	{
		PostQueuedCompletionStatus(m_pIOCP->m_IOCP, 0, (ULONG_PTR)IOCP_EXIT_FLAG, NULL);
	}

	SetEvent(m_hQuitEvent);    ///< ����Ϊ���źţ�����֪ͨ�߳��˳�

	///< һֱ�ȴ��������߳̾�����ź�
	WaitForMultipleObjects(m_vcthThread.size(), &m_vcthThread[0], TRUE, INFINITE);
	
	std::for_each(m_vcthThread.begin(), m_vcthThread.end(),
		[](HANDLE& hThread)
		{
			CloseHandle(hThread);
			hThread = NULL;
	});

	m_pIOCP->ReleaseIOCP();

	return 0;
}

DWORD WINAPI CThreadPoolSelfCreate::WorkThread( LPVOID lpThreadParameter )
{
	CThreadPoolSelfCreate* pThis = (CThreadPoolSelfCreate*)lpThreadParameter;

	return IsSuccessed(pThis->DoTask());
}

RET_STATUS CThreadPoolSelfCreate::DoTask()
{
	DWORD dwThreadID = ::GetCurrentThreadId();
	while (WAIT_OBJECT_0 != WaitForSingleObject(m_hQuitEvent, 0))
	{
		OVERLAPPED           *pOverlapped = NULL;
		LPPER_IO_CONTEXT     pSockCtx = NULL;
		DWORD                dwBytesTransfered = 0;
		LONG_PTR		     nCompleteKey = 0;

		BOOL bRet = GetQueuedCompletionStatus(m_pIOCP->m_IOCP,
			&dwBytesTransfered, 
			(PULONG_PTR)&nCompleteKey,
			&pOverlapped,
			INFINITE);

		LOG(INFO)<<"ThreadID:"<<dwThreadID;

		if (nCompleteKey != NULL && nCompleteKey == IOCP_EXIT_FLAG)
		{
			///< IOCP �˳���ʶ
			break;
		}

		if (pOverlapped == NULL && !bRet)
		{
			/************************************************************************/
			/* If * lpOverlapped is NULL and the function does not dequeue a completion packet from the completion port, 
			/* the return value is zero. The function does not store information in the variables pointed to
			/* by the lpNumberOfBytes and lpCompletionKey parameters. To get extended error information, 
			/* call GetLastError. If the function did not dequeue a completion packet because the wait timed out, GetLastError returns WAIT_TIMEOUT.                                                                      */
			/************************************************************************/
			continue;
		} 

		if (pOverlapped != NULL)
		{
			pSockCtx = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, overLapped);
			if (!pSockCtx->pAsyncIO->OnAsyncExcute(bRet, dwBytesTransfered, (void*)pSockCtx))
			{	
				delete pSockCtx;
				pSockCtx = NULL;
			}
		}
		
	}

	LOG(WARNING)<<"�߳�:"<<dwThreadID<<"�˳�";

	return 0;
}

RET_STATUS CThreadPoolSelfCreate::BindHandleWithIOCP( HANDLE FileHandle, ULONG_PTR CompletionKey )
{
	if (m_pIOCP.get() != NULL)
	{
		m_pIOCP->AssociateWithIOCP(FileHandle, CompletionKey);
	}

	return 0;
}

///< ����GetQueuedCompletionStatus����ֵ���ദ�����
		//if (pOverlapped == NULL && !bRet)
		//		{
		//			/************************************************************************/
		//			/* If * lpOverlapped is NULL and the function does not dequeue a completion packet from the completion port, 
		//			/* the return value is zero. The function does not store information in the variables pointed to
		//			/* by the lpNumberOfBytes and lpCompletionKey parameters. To get extended error information, 
		//			/* call GetLastError. If the function did not dequeue a completion packet because the wait timed out, GetLastError returns WAIT_TIMEOUT.                                                                      */
		//			/************************************************************************/
		//			continue;
		//		} 
		//else if (pOverlapped != NULL && !bRet)
		//{
		//	/************************************************************************/
		//	/* If * lpOverlapped is not NULL and the function dequeues a completion packet for a failed I/O operation from the completion port, 
		//	/* the return value is zero. The function stores information in the variables pointed to by lpNumberOfBytes, lpCompletionKey, 
		//	/* and lpOverlapped. To get extended error information, call GetLastError. 
		//	/************************************************************************/

		//	pSockCtx = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, overLapped);

		//	DWORD dwError = GetLastError();
		//	if (ERROR_ABANDONED_WAIT_0 == dwError)
		//	{
		//		///< the completion port closed
		//		///<TODO ���IOCP �˿ڱ��رյĴ���
		//		break;
		//	}

		//	pSockCtx->oper = IO_ERROR;
		//	pSockCtx->pAsyncIO->OnAsyncExcute((void*)pSockCtx);
		//	pSockCtx->pAsyncIO->Release();
		//	delete pSockCtx;
		//	pSockCtx = NULL;
		//}
		//else if( pOverlapped != NULL && bRet)
		//{
		//	/************************************************************************/
		//	/* If the function dequeues a completion packet for a successful I/O operation from the completion port, 
		//	/* the return value is nonzero. The function stores information in the variables pointed to by the lpNumberOfBytes, 
		//	/* lpCompletionKey, and lpOverlapped parameters. 
		//	/************************************************************************/
		//	pSockCtx = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, overLapped);

		//	if (0 == dwBytesTransfered)
		//	{
		//		DWORD dwError = GetLastError();
		//		IO_OPER ioBefore =  pSockCtx->oper;
		//		pSockCtx->oper = IO_ERROR;
		//		
		//		pSockCtx->pAsyncIO->OnAsyncExcute((void*)pSockCtx);

		//		if (IO_ACCEPT != ioBefore)
		//		{
		//			pSockCtx->pAsyncIO->Release();
		//			delete pSockCtx;
		//			pSockCtx = NULL;
		//		}
		//	}
		//	else
		//	{
		//		pSockCtx->pAsyncIO->OnAsyncExcute((void*)pSockCtx);
		//	}
		//	
		//}