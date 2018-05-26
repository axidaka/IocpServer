#pragma once

#include "t_reader_writer_lock.h"
#include "stldefine.h"
#include "protocol.h"

class IObserver;

/*
@brief  �۲���ģʽ�пɹ۲��߽ӿ���
@duty   1.��ӡ�ɾ���۲���
		2.������ĳһ���۲��߷���״̬��Ϣ
		3.�����й۲��߹㲥״̬��Ϣ
		4.���ö���Ҫ����ʱ����Ҫ�����еĹ۲��߷��ͱ�����������Ϣ
@author zhqsong
#date   2013/12/14
*/

class  ISubject
{

public:

	ISubject();
	virtual ~ISubject();

	void AddObserver(IObserver* pObserver);

	void RemoteOberver(IObserver* pObserver);
	
	bool Broadcast(const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2);
	
	bool Notisfy(IObserver* const pObserver, const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2);

protected:

	mutable scoped_reader_writer_lock m_rwSetObserver;
	std::set<IObserver*>  m_setObserver;
};