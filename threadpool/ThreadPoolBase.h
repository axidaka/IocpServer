#pragma once

#include "ret_status.h"
#include "IOCP_Struct.h"
#include <Windows.h>
#define MAX_THREAD_NUM 24

/*
@brief  �̳߳ػ���
		������1.�̳߳صĴ��� 
	          2.��socket���߳�
@author zhqsong
@date   2013/11/05
*/

class CThreadPoolBase
{

public:

	CThreadPoolBase(void){}
	virtual ~CThreadPoolBase(void){}

	virtual RET_STATUS CreateThreadPool(int nThreads) = 0;
	virtual RET_STATUS DestroyThreadPool() = 0;

	/////< Ͷ��I/O��������
	//virtual RET_STATUS PostIOTask(void* _handle, void* &_hKey, void* &pWparam) = 0;

	virtual RET_STATUS BindHandleWithIOCP(HANDLE FileHandle, ULONG_PTR CompletionKey) = 0;

	///< ִ���̺߳���
	virtual RET_STATUS DoTask() = 0;
};

