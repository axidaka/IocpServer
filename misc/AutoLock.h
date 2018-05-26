#pragma once

#include <Windows.h>

/*
@brief ��װ�ٽ�������
@author zhqsong
@date   2013/12/26
*/

class CAutoCriticalSection
{

public:
	CAutoCriticalSection();
	~CAutoCriticalSection();

	void Lock();
	void Unlock();

private:

	CRITICAL_SECTION m_cs;
};

///////////////////////////////////////////////////////////////
/*
@brief  ��װ�ٽ������Զ��������캯��������������������
@author zhqsong 
@date   2013/12/26
*/
class CAutoLock
{
public:

	CAutoLock(CAutoCriticalSection &lock);
	~CAutoLock(void);

private:

	CAutoCriticalSection &m_autoLock;
};
