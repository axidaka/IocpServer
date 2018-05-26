#pragma once

#include <Windows.h>

/*
@brief 分装临界区的锁
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
@brief  分装临界锁的自动锁，构造函数加锁，析构函数解锁
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
