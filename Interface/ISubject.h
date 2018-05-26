#pragma once

#include "t_reader_writer_lock.h"
#include "stldefine.h"
#include "protocol.h"

class IObserver;

/*
@brief  观察者模式中可观察者接口类
@duty   1.添加、删除观察者
		2.单独向某一个观察者发送状态消息
		3.向所有观察者广播状态消息
		4.当该对象要销毁时，需要向所有的观察者发送本对象销毁消息
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