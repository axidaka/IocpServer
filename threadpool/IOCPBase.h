#pragma once

#include <windows.h>

/**
@brief    IO��ɶ˿ڻ���
@function 1.IOCP�Ĵ�������handle�İ�
          2.
author    zhengqingsong
@datea    2013/11/22
*/

class CIOCPBase
{
	friend class CThreadPoolSelfCreate;

public:

	CIOCPBase(void);
	virtual ~CIOCPBase(void);

	virtual BOOL CreateNewIOCP(DWORD NumberOfConcurrentThreads);

	virtual BOOL ReleaseIOCP();

	virtual BOOL AssociateWithIOCP(HANDLE FileHandle, ULONG_PTR CompletionKey);

protected:

	HANDLE m_IOCP;
};

