#pragma once

#include "stldefine.h"
#include "AsyncListen.h"
#include "AsyncRemoteClient.h"
#include "IObserver.h"
#include "IOCP_Struct.h"

class IAsyncIO;

/**
@brief  ���������
@duty   1.ά���ͻ����б�
        2.��Ϊ�۲�ģʽ�еĹ۲��ߣ���֪ͨ ��ӡ�ɾ�� �ͻ���
@authro zhqsong
@date   2013/12/14
*/

class CNetServer : public IObserver
{
	
public:

	CNetServer();
	virtual ~CNetServer();

	RET_STATUS InitServer();
	
	RET_STATUS UninitServer();
	
	void StartListen(const char *pListenIP, const UINT32 port);
	void AddListener(IAsyncIO* pNewListener);

	void AddRemoteClient(IAsyncIO* pRemoteClient, LPPER_IO_CONTEXT pIOCtx);

	void DeleteRemoteClient(IAsyncIO* pRemoteClient, SOCKET* pRemoteSockKey);

	///< Override From IObserver
	virtual void Notify(const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2) ;

private:

	///< ����Ķ���������ָ���ԭ���ǣ��Ѿ������ü���
	mutable scoped_reader_writer_lock m_rwListenerSet;
	std::set<IAsyncIO*>  m_setListener;

	mutable scoped_reader_writer_lock m_rwsRemoteClientSet;
	//std::set<IAsyncIO*>  m_setRemoteClient;
	std::map<IAsyncIO*, std::set<LPPER_IO_CONTEXT>> m_mapRemoteClient2IOSet;

	std_shared_ptr<CThreadPoolBase> m_pThPool;     ///< �̳߳�
};