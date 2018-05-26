#pragma once

#include "ThreadPoolBase.h"
#include "ret_status.h"
#include "stldefine.h"
#include "IOCPBase.h"
#include "protocol.h"

/**
@brief  ʹ��API�������̳߳�
@author zhqsong
@date   2013/11/19
**/

class CThreadPoolSelfCreate : public CThreadPoolBase
{

public:

	CThreadPoolSelfCreate(std_shared_ptr<CIOCPBase> pIOCPBase);
	//CThreadPoolSelfCreate(void);
	
	virtual~ CThreadPoolSelfCreate(void);

	///< override from CThreadPoolBase
	virtual RET_STATUS CreateThreadPool(int nThreads);
	virtual RET_STATUS DestroyThreadPool();

	virtual RET_STATUS BindHandleWithIOCP(HANDLE FileHandle, ULONG_PTR CompletionKey);

	/////< Ͷ��I/O��������
	//virtual RET_STATUS PostIOTask(void* _handle, void* &_hKey, void* &pWparam);

	virtual RET_STATUS DoTask();

private:

	HANDLE              m_hQuitEvent;   ///< ֪ͨ�߳��˳����¼���������ź��˳��������ִ��
	std::vector<HANDLE> m_vcthThread; 

	static DWORD WINAPI WorkThread(LPVOID lpThreadParameter);

private:

	std_shared_ptr<CIOCPBase>  m_pIOCP;
};