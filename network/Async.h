#ifndef _ASYNC_H_
#define _ASYNC_H_

#include "ret_status.h"
#include "vasyncerrorcode.h"

typedef unsigned int UINT;
typedef int    BOOL;
typedef unsigned long DWORD;

/*
@brief  引用次数接口类，在引用次数为0时释放对象，
		避免内存泄露。作用类似于智能指针
@author zhengqingsong
@date   2013/10/30
*/

class IRef
{
public:

	virtual void AddRef() = 0;
	virtual void Release() = 0;

protected:

	virtual ~IRef(){};
};

/*
@brief 异步IO接口类
@duty  1.实现引用计数增减，计数为0时释放对象
	   2.定义异步操作通知函数
@author  zhqsong
@date    2013/11/1
*/
class IAsyncIO : public IRef
{

public:

	IAsyncIO(void);
	virtual ~IAsyncIO(void);

	///< Override From IRef
	virtual void AddRef();
	virtual void Release();


	///< Overridable Notification Functions
	virtual bool OnAsyncExcute(BOOL bRet, DWORD dwBytesTransfered, void* pParam) = 0;

private:

	long m_lRefCount;    ///< 引用次数
};

/*
@brief   异步Socket接口类
		定义基本的虚函数
@author  zhqsong
@date    2013/11/1
*/

class IAsyncSocket : public IAsyncIO
{
public:

	IAsyncSocket(void);
	virtual ~IAsyncSocket(void);

	///< 创建、关闭套接字
	virtual RET_STATUS OpenSocket(const char* pStrIP, const UINT nPort, const bool bTcp = true) = 0;
	virtual RET_STATUS OpenSocket(const wchar_t* pWStrIP, const UINT nPort, const bool bTcp = true) = 0;
	virtual RET_STATUS CloseSocket() = 0;
};

#endif //_ASYNC_H_

