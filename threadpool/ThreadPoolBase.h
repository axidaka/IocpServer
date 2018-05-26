#pragma once

#include "ret_status.h"
#include "IOCP_Struct.h"
#include <Windows.h>
#define MAX_THREAD_NUM 24

/*
@brief  线程池基类
		操作：1.线程池的创建 
	          2.绑定socket到线程
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

	/////< 投递I/O操作任务
	//virtual RET_STATUS PostIOTask(void* _handle, void* &_hKey, void* &pWparam) = 0;

	virtual RET_STATUS BindHandleWithIOCP(HANDLE FileHandle, ULONG_PTR CompletionKey) = 0;

	///< 执行线程函数
	virtual RET_STATUS DoTask() = 0;
};

