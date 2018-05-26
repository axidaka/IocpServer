#include "stdafx.h"
#include "AsyncListen.h"
#include "AsyncRemoteClient.h"
#include "primitives.h"
#include <comutil.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comsuppw.lib ")

#define  JUDGE_STATUS(pFnCall) \
	status = pFnCall;\
	if (IsFailed(status))\
	{\
		return status; \
	}\

//CAsyncListen::CAsyncListen(void)
//	:m_socketListen(INVALID_SOCKET)
//	,m_lpfnAcceptEx(NULL)
//{
//}

CAsyncListen::CAsyncListen( std_shared_ptr<CThreadPoolBase> pThPool )
	:m_pThPool(pThPool)
	,m_ListenSocket(INVALID_SOCKET)
	,m_lpfnAcceptEx(NULL)
	,m_lpfnGetAcceptExSockAddrs(NULL)
	,m_lpfnDisconnectEx(NULL)
{

}


CAsyncListen::~CAsyncListen(void)
{
}

RET_STATUS CAsyncListen::OpenSocket( const char* pStrIP, const UINT nPort, const bool bTcp /*= true*/)
{
	CloseSocket();

	m_ListenSocket = WSASocket(AF_INET, bTcp ? SOCK_STREAM : SOCK_DGRAM, 
		bTcp ? IPPROTO_TCP : IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == m_ListenSocket)
	{
		return ERR_64CODE_SYSTEM_API(WSAGetLastError());
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	if (NULL != pStrIP)
	{
		server.sin_addr.s_addr = inet_addr(pStrIP);
	}
	else
	{
		server.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	server.sin_port = htons(nPort);

	if (SOCKET_ERROR == bind(m_ListenSocket, (SOCKADDR*)&server, sizeof(server)))
	{
		auto nLastError = WSAGetLastError();
		closesocket(m_ListenSocket);

		return ERR_64CODE_SYSTEM_API(nLastError);
	}
	
	return ERR_64CODE_VASYNC_SUCCESS;

}

RET_STATUS CAsyncListen::OpenSocket( const wchar_t* pWStrIP, const UINT nPort, const bool bTcp /*= true*/ )
{
	_bstr_t bstrIP(pWStrIP);
	return	OpenSocket((char*)bstrIP, nPort, bTcp);
}

RET_STATUS CAsyncListen::CloseSocket()
{
	if (INVALID_SOCKET != m_ListenSocket)
	{
		shutdown(m_ListenSocket, SD_BOTH);
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;

		CAutoLock autolock(m_autoLock);
		m_lstReuseAcceptCtx.clear();
		m_lstCurrentUseAcceptCtx.clear();
	}

	return ERR_64CODE_VASYNC_SUCCESS;
}

RET_STATUS CAsyncListen::Listen()
{
	if (INVALID_SOCKET == m_ListenSocket)
		return ERR_64CODE_VASYNC_INVALIDSOCK;

	///< ��ʼ����
	if (SOCKET_ERROR == listen(m_ListenSocket, SOMAXCONN))
	{
		return ERR_64CODE_SYSTEM_API(GetLastError());
	}

	if (m_pThPool.get() != NULL)
	{
		///TODO
	}
	
	RET_STATUS status = ERR_64CODE_VASYNC_SUCCESS;

	///< �󶨼����׽��ֵ�IOCP
	JUDGE_STATUS(m_pThPool->BindHandleWithIOCP((HANDLE)m_ListenSocket, (ULONG_PTR)0));

	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;

	LOAD_WINSOCK2_FUNC(m_ListenSocket, GuidAcceptEx, m_lpfnAcceptEx)
	LOAD_WINSOCK2_FUNC(m_ListenSocket, GuidGetAcceptExSockAddrs, m_lpfnGetAcceptExSockAddrs)
	LOAD_WINSOCK2_FUNC(m_ListenSocket, GuidDisconnectEx, m_lpfnDisconnectEx)

	// ���� ������AcceptEx���׽��֣�����Accept_overlapped
	for (int i = 0; i < ACCEPT_NUM; ++ i)
	{
		///< �½�һ��PER_IO_CONTEXT
		PER_IO_CONTEXT* pAcceptCtx = NULL;

		RET_STATUS status;

		JUDGE_STATUS(ReturnAccpetCtx(pAcceptCtx));
		
		JUDGE_STATUS(PostAcceptEx(pAcceptCtx));
		
	}
	
	return 0;
}

RET_STATUS CAsyncListen::ReturnAccpetCtx( PER_IO_CONTEXT* &pIOCtx )
{
	CAutoLock autolock(m_autoLock);

	if (0 != m_lstReuseAcceptCtx.size())
	{
		auto pAcceptCtx = m_lstReuseAcceptCtx.begin();
		
		m_lstCurrentUseAcceptCtx.push_back(std_auto_ptr<PER_IO_CONTEXT>(pAcceptCtx->release()));
		
		m_lstReuseAcceptCtx.pop_front();

		pIOCtx = (m_lstCurrentUseAcceptCtx.back()).get();

		return ERR_64CODE_VASYNC_SUCCESS;
	}
	else
	{
		pIOCtx = new PER_ACCEPT_CONTEXT(this);
		
		m_lstCurrentUseAcceptCtx.push_back(std_auto_ptr<PER_IO_CONTEXT>(pIOCtx));

		return ERR_64CODE_VASYNC_SUCCESS;
	}
}

RET_STATUS CAsyncListen::PostAcceptEx( PER_IO_CONTEXT* &pIOCtx )
{
	///< ����ת��
	auto pAcceptCtx = dynamic_cast<PER_ACCEPT_CONTEXT*>(pIOCtx);
	assert(pAcceptCtx != NULL);
	
	if (INVALID_SOCKET == pAcceptCtx->sAcceptSocket)
	{
		pAcceptCtx->sAcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
			NULL, 0, WSA_FLAG_OVERLAPPED);

		if (INVALID_SOCKET == pAcceptCtx->sAcceptSocket)
		{
			///< TODO
		}
	}
	else
	{
		///< �������ø�socket
		this->m_lpfnDisconnectEx(pAcceptCtx->sAcceptSocket, NULL, TF_REUSE_SOCKET, 0);
	}

	DWORD dwBytes = 0;  
	auto nRet = m_lpfnAcceptEx(m_ListenSocket, pAcceptCtx->sAcceptSocket, pAcceptCtx->data, 
		pAcceptCtx->wbuf.len  - ((sizeof(SOCKADDR_IN)+16)*2), sizeof(SOCKADDR_IN) +16, 
		sizeof(SOCKADDR_IN) + 16,  &dwBytes, &pAcceptCtx->overLapped);

	RET_STATUS status = ERR_64CODE_VASYNC_SUCCESS;

	if (!nRet && WSA_IO_PENDING != WSAGetLastError())
	{
		status =  ERR_64CODE_SYSTEM_API(WSAGetLastError());
	}

	return status;
}

bool CAsyncListen::OnAsyncExcute(BOOL bRet, DWORD dwBytesTransfered, void* pParam)
{
	RET_STATUS status = ERR_64CODE_VASYNC_SUCCESS;

	LPPER_IO_CONTEXT pIOCtx = (LPPER_IO_CONTEXT)pParam;
	assert(pIOCtx != NULL);
	
	if (bRet)
	{
		if (0 != dwBytesTransfered)
		{
			///< ����Զ������ 

			/************************************************************************/
			/* 1.DoAcceptEx����Զ������
			/* 2.Ͷ���µ�Accept����,ǰ�����ʧ�ܣ�Ҫ�������ø�socket
			/************************************************************************/
		    status = DoAcceptEx(pIOCtx);
			if (IsFailed(status))
			{
				status = PostAcceptEx(pIOCtx);
			}
			else
			{
				PER_IO_CONTEXT* pAcceptCtx = NULL;
				ReturnAccpetCtx(pAcceptCtx);
				status = PostAcceptEx(pAcceptCtx);
			}
		}
		else
		{
			///< �ͻ��������Ϻ�ֱ�ӶϿ���δ��������
			/************************************************************************/
			/* 1.����Ͷ�ݸ�AcceptCtx ����
			/************************************************************************/
			LOG(WARNING)<<"Զ�������쳣�Ͽ���δ�������ݣ�";
			status = PostAcceptEx(pIOCtx);
		}
	}
	else
	{
		status = DoIOError(pIOCtx);
	}

	return (IsSuccessed(status));
}

RET_STATUS CAsyncListen::DoIOError( LPPER_IO_CONTEXT &pIOCtx )
{
	DWORD dwError = GetLastError();

	LOG(WARNING)<<"����I/O���������룺"<<dwError;
	CAutoLock autolock(m_autoLock);

	auto iter_lstEnd = m_lstCurrentUseAcceptCtx.end();
	for (auto iter_lst = m_lstCurrentUseAcceptCtx.begin(); iter_lst != iter_lstEnd; )
	{
		if (pIOCtx == iter_lst->get())
		{
			iter_lst->release();
			m_lstCurrentUseAcceptCtx.erase(iter_lst);
			break;
		}
		else
		{
			iter_lst ++;
		}
	}

	return ERR_64CODE_SYSTEM_API(ERROR_OPERATION_ABORTED);
}

RET_STATUS CAsyncListen::DoAcceptEx( LPPER_IO_CONTEXT &pIOCtx )
{
	/************************************************************************/
	/* 1.GetAcceptExSockAddrs����Զ������ Get IP Port 
	/* 2.��remotesock �󶨵�IOCP���������������)
	/* 3.����Զ�̿ͻ����󣨴���remotesock),֪ͨ�۲���,����ʼ��Ͷ��Recv����
	/************************************************************************/

	LPPER_ACCEPT_CONTEXT pAcceptCtx = dynamic_cast<LPPER_ACCEPT_CONTEXT>(pIOCtx);
	assert(pAcceptCtx != NULL);

	SOCKADDR_IN* pLocalSockaddr = NULL;
	SOCKADDR_IN* pRemoteSockaddr = NULL;
	int nLocalSockaddrLength = 0;
    int nRemoteSockaddrLength = 0;

	///< step 1
	this->m_lpfnGetAcceptExSockAddrs(pAcceptCtx->wbuf.buf, pAcceptCtx->wbuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) +16, 
		(LPSOCKADDR*)&pLocalSockaddr, &nLocalSockaddrLength, 
		(LPSOCKADDR*)&pRemoteSockaddr, &nRemoteSockaddrLength);

	///<  �����׽���
	setsockopt(pAcceptCtx->sAcceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char*)&(m_ListenSocket), sizeof(m_ListenSocket));

	LOG(WARNING)<<"Զ�̿ͻ��˽���"<<inet_ntoa(pRemoteSockaddr->sin_addr)<<":"<<ntohs(pRemoteSockaddr->sin_port)<<"---������Ϣ:"<<pIOCtx->wbuf.buf;

	RET_STATUS status;
	///< step 2
	JUDGE_STATUS(m_pThPool->BindHandleWithIOCP((HANDLE)pAcceptCtx->sAcceptSocket, (ULONG_PTR)0));

	///< step 3
	//LOG(INFO)<<"�����µ�Զ�̶���";
	memcpy(&pAcceptCtx->RemoteSockAddr, pRemoteSockaddr, sizeof(SOCKADDR_IN));

	CAsyncRemoteClient* pNewRemoteClient = new CAsyncRemoteClient(pAcceptCtx->sAcceptSocket, pAcceptCtx->RemoteSockAddr);
	IObserver* pThis = (IObserver*)this;
	pNewRemoteClient->AddObserver(pThis);

	LPPER_IO_CONTEXT pIORecv = new PER_IO_CONTEXT(pNewRemoteClient, IO_READ);
	pNewRemoteClient->PostRecv(pIORecv);

	Broadcast(NOTISFY_MSG_SERVER_NEWCLIENT, (void*)pNewRemoteClient, (void*)pIORecv);

	pNewRemoteClient->Release();   ///< ע������Ҫ�ͷ�һ�Σ�����������NetServer I/O Ctx�ͷ�

	return ERR_64CODE_VASYNC_SUCCESS;
}

RET_STATUS CAsyncListen::UpdateAcceptCtxList( SOCKET RemoteSock )
{
	/************************************************************************/
	/* 1.�Ͽ�����socket
	/* 2.����I/O���ݴ� m_lstCurrentUseAcceptCtxɾ��
	/* 3.����I/O����ѭ��ʹ�ã����� m_lstReuseAcceptCtx
	/* 4.֪ͨNetServer���¿ͻ����б����ܴ��ڰ�Socket���Ҳ���)
	/************************************************************************/

	///<1 step
	m_lpfnDisconnectEx(RemoteSock, NULL, TF_REUSE_SOCKET, 0);

	CAutoLock autolock(m_autoLock);

	///< 2 step
	auto iter_find = std::find_if(m_lstCurrentUseAcceptCtx.begin(), m_lstCurrentUseAcceptCtx.end(),
		[&](const std_auto_ptr<PER_IO_CONTEXT>& pItem)->bool{
			auto pItemTmp = dynamic_cast<PER_ACCEPT_CONTEXT*>(pItem.get());
			if (pItemTmp != NULL)
			{
				return (RemoteSock == pItemTmp->sAcceptSocket);
			}
			return false;
	});

	if (m_lstCurrentUseAcceptCtx.end() != iter_find)
	{
		///< 3 step
		m_lstReuseAcceptCtx.push_back(std_auto_ptr<PER_IO_CONTEXT>(iter_find->release()));    

		m_lstCurrentUseAcceptCtx.erase(iter_find);
	}

	///< 4 step
	Broadcast(NOTISFY_MSG_CLIENT_DISCONNECTCLIENT, NULL, (void*)&RemoteSock);

	return ERR_64CODE_VASYNC_SUCCESS;
}

void CAsyncListen::Notify( const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2 )
{
	switch(iMessage)
	{
	case NOTISFY_MSG_CLIENT_DISCONNECTCLIENT:  ///< �ͻ��˶Ͽ�
		{
			/************************************************************************/
			/* 1.�Ͽ�����socket
			/* 2.����I/O���ݴ� m_lstCurrentUseAcceptCtxɾ��
			/* 3.����I/O����ѭ��ʹ�ã����� m_lstReuseAcceptCtx
			/************************************************************************/
			
			if (pParam2 != NULL)
			{
				UpdateAcceptCtxList(*(SOCKET*)pParam2);
			}
			
		}
		break;
	case NOTISFY_MSG_SERVER_DISCONNECTCLIENT:  ///< ����������Ͽ�����
		{
			SOCKET* pRemoteSock = (SOCKET*)pParam2;
			Broadcast(NOTISFY_MSG_SERVER_DISCONNECTCLIENT, NULL, (void*)pRemoteSock);
		}
		break;
	default:
		break;
	}
}
