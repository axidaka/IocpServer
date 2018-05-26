#include "stdafx.h"
#include "AsyncRemoteClient.h"

CAsyncRemoteClient::CAsyncRemoteClient( SOCKET & _RemoteSock, const SOCKADDR_IN & _RemoteSockAddr )
	:m_RemoteSocket(_RemoteSock)
	,m_RemoteSockaddr(_RemoteSockAddr)
{

}


CAsyncRemoteClient::~CAsyncRemoteClient(void)
{
}

bool CAsyncRemoteClient::OnAsyncExcute(BOOL bRet, DWORD dwBytesTransfered, void* pParam)
{
	RET_STATUS status = ERR_64CODE_VASYNC_SUCCESS;

	LPPER_IO_CONTEXT pIOCtx = (LPPER_IO_CONTEXT)pParam;
	assert(pIOCtx != NULL);

	if (bRet)
	{
		if (0 != dwBytesTransfered)
		{
			switch(pIOCtx->oper)
			{
			case IO_READ:
				{
					status = DoRecv(pIOCtx);
				}
				break;
			case IO_WRITE:
				break;
			default:
				break;
			}
		}
		else
		{
			status = DoIOError(pIOCtx);
		}
	} 
	else
	{ 
		status = DoIOError(pIOCtx);
	}
	
	return (IsSuccessed(status));
}

RET_STATUS CAsyncRemoteClient::PostRecv( LPPER_IO_CONTEXT pIOCtx /*= NULL*/ )
{
	bool bNew = false;
	if (pIOCtx == NULL)
	{
		pIOCtx = new PER_IO_CONTEXT(this, IO_READ);
		bNew = true;
	}
	else
	{
		pIOCtx->ResetBuf();
	}

	///< Ͷ��Recv����
	int nRet = WSARecv(m_RemoteSocket, 
		&pIOCtx->wbuf,
		1,
		&pIOCtx->wbuf.len,
		&pIOCtx->flags,
		&pIOCtx->overLapped,
		NULL);

	if (SOCKET_ERROR == nRet && WSA_IO_PENDING != WSAGetLastError())
	{
		if (bNew)
		{
			delete pIOCtx;
			pIOCtx = NULL;
		}
	
		return ERR_64CODE_SYSTEM_API(WSAGetLastError());
	}

	return ERR_64CODE_VASYNC_SUCCESS;
}

RET_STATUS CAsyncRemoteClient::DoRecv( LPPER_IO_CONTEXT pIOCtx )
{
	LOG(WARNING)<<"�ͻ��ˣ�"<<inet_ntoa(m_RemoteSockaddr.sin_addr)<<":"<<ntohs(m_RemoteSockaddr.sin_port)<<":������Ϣ��"<<pIOCtx->wbuf.buf;

	return PostRecv(pIOCtx);
}

RET_STATUS CAsyncRemoteClient::DoIOError( LPPER_IO_CONTEXT &pIOCtx )
{
	DWORD dwError = GetLastError();
	LOG(WARNING)<<"�ͻ���:"<<inet_ntoa(m_RemoteSockaddr.sin_addr)<<":"<<ntohs(m_RemoteSockaddr.sin_port)<<"I/O���������룺"<<dwError;

	if (ERROR_OPERATION_ABORTED == dwError)
	{
		///< ����������Ͽ�����
		Broadcast(NOTISFY_MSG_SERVER_NEWCLIENT, (void*)this, (void*)&m_RemoteSocket);
	}

	///< �ͻ������� ����  �ر�
	Broadcast(NOTISFY_MSG_CLIENT_DISCONNECTCLIENT, (void*)this, (void*)&m_RemoteSocket);
	return ERR_64CODE_VASYNC_CONNECTIONCLOSED;
}

RET_STATUS CAsyncRemoteClient::CloseSocket()
{
	LOG(WARNING)<<"�����Ͽ��ͻ�������: "<<inet_ntoa(m_RemoteSockaddr.sin_addr)<<":"<<ntohs(m_RemoteSockaddr.sin_port);

	shutdown(m_RemoteSocket, SD_BOTH);
	closesocket(m_RemoteSocket);
	m_RemoteSocket = INVALID_SOCKET;

	return ERR_64CODE_VASYNC_SUCCESS;
}
