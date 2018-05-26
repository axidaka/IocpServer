#include "stdafx.h"
#include "NetServer.h"
#include "AsyncListen.h"
#include "ThreadPoolSelfCreate.h"
#include "protocol.h"

CNetServer::CNetServer()
	:IObserver()
{

}

CNetServer::~CNetServer()
{

}

RET_STATUS CNetServer::InitServer()
{
	///< 初始化线程池 IOCP
	m_pThPool = std_shared_ptr<CThreadPoolBase>(new CThreadPoolSelfCreate(std_shared_ptr<CIOCPBase>(new CIOCPBase)));
	
	return (m_pThPool->CreateThreadPool(THREADS_NUM));
}

RET_STATUS CNetServer::UninitServer()
{
	/************************************************************************/
	/* 1.退出线程池
	/* 2.删除客户连接列表，并释放I/O操作
	/* 3.删除监听对象列表，并释放I/O操作
	/************************************************************************/
	assert(m_pThPool != NULL);
	m_pThPool->DestroyThreadPool();

	{
		scoped_reader_writer_lock::scoped_lock_writer writer(m_rwsRemoteClientSet);
		std::for_each(m_mapRemoteClient2IOSet.begin(), m_mapRemoteClient2IOSet.end(), 
			[](std::pair<IAsyncIO*, std::set<LPPER_IO_CONTEXT>> MapItem){

				auto pRemoteClient = dynamic_cast<CAsyncRemoteClient*>(MapItem.first);
				if (pRemoteClient != NULL)
				{
					std::for_each(MapItem.second.begin(), MapItem.second.end(),
						[](LPPER_IO_CONTEXT pIOCtx){
							delete pIOCtx;
							pIOCtx = NULL;
					});
					MapItem.second.clear();

					pRemoteClient->CloseSocket();
					pRemoteClient->Release();
				}
		});

		m_mapRemoteClient2IOSet.clear();
	}

	{
		scoped_reader_writer_lock::scoped_lock_writer writer(m_rwListenerSet);
		std::for_each(m_setListener.begin(), m_setListener.end(),
			[](IAsyncIO* pAsyncIO){
				auto pListener = dynamic_cast<CAsyncListen*>(pAsyncIO);
				if (pListener != NULL)
				{
					pListener->CloseSocket();
					pListener->Release();
				}
		});

		m_setListener.clear();
	}
	

	

	return ERR_64CODE_VASYNC_SUCCESS;
}

void CNetServer::StartListen( const char *pListenIP, const UINT32 port )
{
	//CIOCPBase IOCPObject;
	CAsyncListen* pListener = new CAsyncListen(m_pThPool);

	RET_STATUS status = pListener->OpenSocket(_T("0.0.0.0"), port, true);
	if (IsSuccessed(status))
	{
		pListener->Listen();
	}
	else
	{
		pListener->Release();
		pListener = NULL;
		return ;
	}

	AddListener(pListener);

	pListener->Release();   ///< 这里也要释放一次，以后交给NetServer I/OCtx

	///< 将NetServer作为观察者添加到监听类，以便通知
	IObserver* pObserver = (IObserver*)this;
	pListener->AddObserver(pObserver);
}

void CNetServer::AddListener( IAsyncIO* pNewListener )
{
	scoped_reader_writer_lock::scoped_lock_writer writer(m_rwListenerSet);
	
	pNewListener->AddRef();
	m_setListener.insert(pNewListener);
}

void CNetServer::AddRemoteClient( IAsyncIO* pRemoteClient, LPPER_IO_CONTEXT pIOCtx )
{
	scoped_reader_writer_lock::scoped_lock_writer writer(m_rwsRemoteClientSet);
	
	pRemoteClient->AddRef();  

	/************************************************************************/
	/*1.远程客户端加入列表前引用计数需要为2
	/*2.客户端断开连接后，自己通知CNetServer删除列表中对象，此时引用计数为1
	/*3.通知完之后再在其他地方删除包含客户端对象的单I/O数据，并调用Release，计数为0，释放对象
	/************************************************************************/
	auto iter_find = m_mapRemoteClient2IOSet.find(pRemoteClient);
	if (m_mapRemoteClient2IOSet.end() == iter_find)
	{
		std::set<LPPER_IO_CONTEXT> IOCtxSet;
		IOCtxSet.insert(pIOCtx);
		m_mapRemoteClient2IOSet.insert(std::make_pair(pRemoteClient, IOCtxSet));
	}
	else
	{
		iter_find->second.insert(pIOCtx);
	}

	LOG(INFO)<<"AddRemoteClient Size:"<<m_mapRemoteClient2IOSet.size();
}

void CNetServer::DeleteRemoteClient( IAsyncIO* pRemoteClient, SOCKET* pRemoteSockKey)
{
	scoped_reader_writer_lock::scoped_lock_writer writer(m_rwsRemoteClientSet);

	///< 先通过指针对象查找
	if (pRemoteClient != NULL)
	{
		auto iter_find = m_mapRemoteClient2IOSet.find(pRemoteClient);
		if (m_mapRemoteClient2IOSet.end() != iter_find)
		{
			pRemoteClient->Release();

			///< 当前情况是客户端主动断开连接的正常处理，I/O操作在线程池中释放
			m_mapRemoteClient2IOSet.erase(iter_find);
		}
	}

	///< 再根据socket查找对象
	if (pRemoteSockKey != NULL)
	{
		auto iter_MapEnd = m_mapRemoteClient2IOSet.end();
		for (auto iter_map = m_mapRemoteClient2IOSet.begin(); iter_map != iter_MapEnd; )
		{
			auto pTmp = dynamic_cast<CAsyncRemoteClient*>(iter_map->first);
			if (pTmp != NULL)
			{
				if (pTmp->m_RemoteSocket == (*pRemoteSockKey))
				{
					pTmp->Release();
					iter_map = m_mapRemoteClient2IOSet.erase(iter_map);
					break;
				}
				else
				{
					++ iter_map;
				}
			}

		}
	}
	
	LOG(INFO)<<"DeleteRemoteClient Size:"<<m_mapRemoteClient2IOSet.size();
}

void CNetServer::Notify( const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2 )
{
	switch(iMessage)
	{
	case NOTISFY_MSG_SERVER_NEWCLIENT:         ///< 新增客户端连接
		{
			LOG(INFO)<<"NetSever 得到新增客户端连接的通知";
			AddRemoteClient((IAsyncIO*)pParam1, (LPPER_IO_CONTEXT)pParam2);
		}
		break; 
	case NOTISFY_MSG_SERVER_DISCONNECTCLIENT:  ///< 服务端主动断开客户端连接
		{
			LOG(INFO)<<"NetSever 主动断开客户端连接";
		}
		break;
	case NOTISFY_MSG_CLIENT_DISCONNECTCLIENT:   ///< 删除客户端连接
		{
			LOG(WARNING)<<"NetSever 得到删除客户端连接的通知";
			DeleteRemoteClient((IAsyncIO*)pParam1, (SOCKET*)pParam2);
		}
		break;
	default:
		break;
	}
}


