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
	///< ��ʼ���̳߳� IOCP
	m_pThPool = std_shared_ptr<CThreadPoolBase>(new CThreadPoolSelfCreate(std_shared_ptr<CIOCPBase>(new CIOCPBase)));
	
	return (m_pThPool->CreateThreadPool(THREADS_NUM));
}

RET_STATUS CNetServer::UninitServer()
{
	/************************************************************************/
	/* 1.�˳��̳߳�
	/* 2.ɾ���ͻ������б����ͷ�I/O����
	/* 3.ɾ�����������б����ͷ�I/O����
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

	pListener->Release();   ///< ����ҲҪ�ͷ�һ�Σ��Ժ󽻸�NetServer I/OCtx

	///< ��NetServer��Ϊ�۲�����ӵ������࣬�Ա�֪ͨ
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
	/*1.Զ�̿ͻ��˼����б�ǰ���ü�����ҪΪ2
	/*2.�ͻ��˶Ͽ����Ӻ��Լ�֪ͨCNetServerɾ���б��ж��󣬴�ʱ���ü���Ϊ1
	/*3.֪ͨ��֮�����������ط�ɾ�������ͻ��˶���ĵ�I/O���ݣ�������Release������Ϊ0���ͷŶ���
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

	///< ��ͨ��ָ��������
	if (pRemoteClient != NULL)
	{
		auto iter_find = m_mapRemoteClient2IOSet.find(pRemoteClient);
		if (m_mapRemoteClient2IOSet.end() != iter_find)
		{
			pRemoteClient->Release();

			///< ��ǰ����ǿͻ��������Ͽ����ӵ���������I/O�������̳߳����ͷ�
			m_mapRemoteClient2IOSet.erase(iter_find);
		}
	}

	///< �ٸ���socket���Ҷ���
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
	case NOTISFY_MSG_SERVER_NEWCLIENT:         ///< �����ͻ�������
		{
			LOG(INFO)<<"NetSever �õ������ͻ������ӵ�֪ͨ";
			AddRemoteClient((IAsyncIO*)pParam1, (LPPER_IO_CONTEXT)pParam2);
		}
		break; 
	case NOTISFY_MSG_SERVER_DISCONNECTCLIENT:  ///< ����������Ͽ��ͻ�������
		{
			LOG(INFO)<<"NetSever �����Ͽ��ͻ�������";
		}
		break;
	case NOTISFY_MSG_CLIENT_DISCONNECTCLIENT:   ///< ɾ���ͻ�������
		{
			LOG(WARNING)<<"NetSever �õ�ɾ���ͻ������ӵ�֪ͨ";
			DeleteRemoteClient((IAsyncIO*)pParam1, (SOCKET*)pParam2);
		}
		break;
	default:
		break;
	}
}


