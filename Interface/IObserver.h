#pragma once

#include "t_reader_writer_lock.h"
#include "stldefine.h"
#include "protocol.h"

class ISubject;

/*
@brief  �۲���ģʽ�еĹ۲�����ӿ�
@duty   1.�ṩ֪ͨ�ӿ�
		2.��ӡ�ɾ�� �ɱ��۲���
		3.�ڽ��յ��ɹ۲��ߵ�֪ͨ����������Ϣʱ����Ҫɾ��
@author zhqsong
@data   2013/12/14
@modify ȥ������2��3   2014/1/6
*/

class  IObserver
{

public:

	IObserver();
	virtual ~IObserver();

	virtual void Notify(const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2) = 0;

};