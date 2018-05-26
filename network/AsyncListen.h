#pragma once

#include <winsock2.h>
#include "mswsock.h"
#include "Async.h "
#include "ThreadPoolBase.h"
#include "ISubject.h"
#include "IObserver.h"
#include "AutoLock.h"

/*
@brief  �첽������
@duty   1.���������׽��֣����󶨵�IOCP
        2.��ΪSubject,�ڿͻ�������ʱ֪ͨ�۲���
		3.��ΪObserver,���տͻ��˶Ͽ�֪ͨ
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

	std_shared_ptr<CThreadPoolBase>        m_pThPool;            ///< �̳߳�


	//CP_KEY* m_pIOCPListenKey;    ///< ���������

	///< ����ʹ��auto_ptr��ԭ���ǣ�ͬһ��IOCtx������ͬʱ���������б�������÷���auto ptr �ص�
	///< ����Ҫע�������auto_ptrָ������б���auto_ptrӵ��Ȩ��û���ˣ��޷��������ʶ���

	CAutoCriticalSection  m_autoLock;

	std::list<std_auto_ptr<PER_IO_CONTEXT>>  m_lstReuseAcceptCtx;      ///< �ɱ�ѭ��ʹ��

	std::list<std_auto_ptr<PER_IO_CONTEXT>>  m_lstCurrentUseAcceptCtx; ///< ��ǰ����ʹ�� 
};

