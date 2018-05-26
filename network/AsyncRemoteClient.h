#pragma once

#include <WinSock2.h>
#include "Async.h"
#include "IOCP_Struct.h"
#include "ISubject.h"

/*
@brief  远程客户端对象类,异步操作
@duty   1.负责接收、发送数据
@author zhqsong
@date   2013/11/19
*/

class CAsyncRemoteClient : public IAsyncIO, public ISubject
{
	friend class CNetServer;      ///< 便于访问私有成员

public:

	CAsyncRemoteClient(SOCKET & _RemoteSock, const SOCKADDR_IN & _RemoteSockAddr);
	virtual ~CAsyncRemoteClient(void);

	///< Override From IAsyncIO
	virtual bool OnAsyncExcute(BOOL bRet, DWORD dwBytesTransfered, void* pParam);

	RET_STATUS PostRecv(LPPER_IO_CONTEXT pIOCtx = NULL);

	RET_STATUS DoRecv(LPPER_IO_CONTEXT pIOCtx);

	RET_STATUS DoIOError(LPPER_IO_CONTEXT &pIOCtx);

	virtual RET_STATUS CloseSocket();

private:

	SOCKET&                   m_RemoteSocket;

	const SOCKADDR_IN&              m_RemoteSockaddr;

	std_shared_ptr<PER_IO_CONTEXT>  m_pIOCtx;
};

