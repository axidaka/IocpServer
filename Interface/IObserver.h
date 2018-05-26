#pragma once

#include "t_reader_writer_lock.h"
#include "stldefine.h"
#include "protocol.h"

class ISubject;

/*
@brief  观察者模式中的观察者类接口
@duty   1.提供通知接口
		2.添加、删除 可被观察者
		3.在接收到可观察者的通知对象销毁消息时，需要删除
@author zhqsong
@data   2013/12/14
@modify 去掉功能2、3   2014/1/6
*/

class  IObserver
{

public:

	IObserver();
	virtual ~IObserver();

	virtual void Notify(const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2) = 0;

};