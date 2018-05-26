#pragma  once

#include "stldefine.h"
#include "primitives.h"
#include "Async.h"
#include <Winsock2.h>

#define  MAX_BUF_LEN 1024

///< ����I/O��������
typedef enum _IO_OPER
{
	IO_ACCEPT,
	IO_CONNECT,
	IO_READ,
	IO_WRITE,
	IO_ERROR,
}IO_OPER,*LPIO_OPER;

/*
@brief ��չ�ص��ṹ�壬��I/O����
@design��IOCP���ص㡰�豸�����I/O��ɶ���Ͷ��I/O���󣬵�I/O������ɺ��̲߳��ϴ�I/O����ȡ��I/O�����Ϣ,����I/O��������ѡ�������
		 ������һ���ص��ϡ�����ģʽ������I/O������ָ���������pIAsyncIO��ȡ��I/O�����Ϣ��ֱ�ӵ��ô������Ĵ�����
*/
typedef struct _OverLappedEx
{
	OVERLAPPED overLapped;
	IO_OPER    oper;
	DWORD      flags;
	WSABUF     wbuf;
	CHAR      data[MAX_BUF_LEN];
	IAsyncIO*  pAsyncIO;   ///< ��ǰI/O�����Ϣ�Ĵ������

	_OverLappedEx(){}      ///< Ĭ�Ϲ���
	_OverLappedEx(IAsyncIO* _pAsyncIO, IO_OPER _oper)
		:pAsyncIO(_pAsyncIO)
		,oper(_oper)
		,flags(0)
	{
		pAsyncIO->AddRef();///< ע��ÿ�θ��ƶ���������ô���
		memset(&overLapped, 0 ,sizeof(overLapped));
		memset(data, 0, MAX_BUF_LEN);
		memset(&wbuf, 0, sizeof(wbuf));
		wbuf.buf = data;
		wbuf.len = MAX_BUF_LEN;
	}

	virtual ~_OverLappedEx()
	{
		pAsyncIO->Release();
	}

	void ResetBuf()
	{
		memset(&overLapped, 0, sizeof(overLapped));
		memset(data, 0, MAX_BUF_LEN);
	}

}PER_IO_CONTEXT, *LPPER_IO_CONTEXT;

#define ACCEPT_BUFFER_SIZE		(sizeof(sockaddr_in) + 16 + sizeof(sockaddr_in) + 16)

///< ���� ��I/O����
typedef struct _Accept_OverLappedEx : public _OverLappedEx
{
	SOCKET       sAcceptSocket;     ///< Accepth����AcceptExʹ�õĿͻ�����
	SOCKADDR_IN  RemoteSockAddr;    ///< Զ�����ӵĵ�ַ��Ϣ

	_Accept_OverLappedEx(IAsyncIO* _pAsyncIO)
		:_OverLappedEx(_pAsyncIO, IO_ACCEPT)
		,sAcceptSocket(INVALID_SOCKET)
	{}
}PER_ACCEPT_CONTEXT, *LPPER_ACCEPT_CONTEXT;

/////< ��������ݣ���IOCPʱ��Keyֵ
//struct IOCP_KEY 
//{
//	SOCKET  selfSocket;   
//	std::list<PER_IO_DATA*> lstPerIOData;
//
//	IOCP_KEY(SOCKET _socket)
//		:selfSocket(_socket)
//	{}
//
//	virtual ~IOCP_KEY()
//	{
//		Delete_IFPtr_List(PER_IO_DATA, lstPerIOData);
//	}
//};
//
//struct IOCP_LISTEN_KEY : public IOCP_KEY
//{
//	std::list<PER_IO_DATA*> lstReusePerIOData;  ///< ѭ��ʹ�õ��׽���
//
//	IOCP_LISTEN_KEY(SOCKET _socket)
//		:IOCP_KEY(_socket)
//	{}
//
//	~IOCP_LISTEN_KEY()
//	{
//		Delete_IFPtr_List(PER_IO_DATA, lstReusePerIOData);
//	}
//};

