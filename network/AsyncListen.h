#pragma once

#include <winsock2.h>
#include "mswsock.h"
#include "Async.h "
#include "ThreadPoolBase.h"
#include "ISubject.h"
#include "IObserver.h"
#include "AutoLock.h"

/*
@brief  异步监听类
@duty   1.创建监听套接字，并绑定到IOCP
        2.作为Subject,在客户端连接时通知观察者
		3.作为Observer,接收客户端断开通知
@author zhqsong
@date   2013/11/19
*/

class CAsyncListen : public IAsyncSocket, public ISubject, public IObserver
{

public:

	//CAsyncListen(void);
	CAsyncListen(std_shared_ptr<CThreadPoolBase> pThPool);
	virtual ~CAsyncListen(void);

	///< Override From IAsyncIO
	virtual bool OnAsyncExcute(BOOL bRet, DWORD dwBytesTransfered, void* pParam);

	///< Override From IAsyncSocket
	virtual RET_STATUS OpenSocket(const char* pStrIP, const UINT nPort, const bool bTcp = true);
	virtual RET_STATUS OpenSocket(const wchar_t* pWStrIP, const UINT nPort, const bool bTcp = true);
	virtual RET_STATUS CloseSocket();

	///< Override From IObserver
	virtual void Notify(const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2);

	///< self define virtual function
	virtual RET_STATUS Listen();

	RET_STATUS ReturnAccpetCtx(LPPER_IO_CONTEXT &pIOCtx);
	RET_STATUS PostAcceptEx(LPPER_IO_CONTEXT &pIOCtx);
	RET_STATUS DoAcceptEx(LPPER_IO_CONTEXT &pIOCtx);
	RET_STATUS DoIOError(LPPER_IO_CONTEXT &pIOCtx);
	RET_STATUS UpdateAcceptCtxList(SOCKET RemoteSock);

private:

	SOCKET m_ListenSocket;

	LPFN_ACCEPTEX                        m_lpfnAcceptEx;;

	LPFN_GETACCEPTEXSOCKADDRS            m_lpfnGetAcceptExSockAddrs; 

	LPFN_DISCONNECTEX					 m_lpfnDisconnectEx;

	std_shared_ptr<CThreadPoolBase>        m_pThPool;            ///< 线程池


	//CP_KEY* m_pIOCPListenKey;    ///< 单句柄数据

	///< 这里使用auto_ptr的原因是：同一个IOCtx不可能同时存在两个列表，这个正好符合auto ptr 特点
	///< 但是要注意如果将auto_ptr指针放入列表，该auto_ptr拥有权就没有了，无法继续访问对象

	CAutoCriticalSection  m_autoLock;

	std::list<std_auto_ptr<PER_IO_CONTEXT>>  m_lstReuseAcceptCtx;      ///< 可被循环使用

	std::list<std_auto_ptr<PER_IO_CONTEXT>>  m_lstCurrentUseAcceptCtx; ///< 当前正被使用 
};

