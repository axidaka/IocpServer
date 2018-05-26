#pragma  once

#include "stldefine.h"
#include "primitives.h"
#include "Async.h"
#include <Winsock2.h>

#define  MAX_BUF_LEN 1024

///< 定义I/O操作类型
typedef enum _IO_OPER
{
	IO_ACCEPT,
	IO_CONNECT,
	IO_READ,
	IO_WRITE,
	IO_ERROR,
}IO_OPER,*LPIO_OPER;

/*
@brief 扩展重叠结构体，单I/O数据
@design：IOCP的特点“设备句柄向I/O完成队列投递I/O请求，当I/O请求完成后，线程不断从I/O队列取出I/O完成信息,根据I/O操作类型选择处理对象”
		 根据这一个特点结合“命令模式”，在I/O请求中指定处理对象pIAsyncIO，取出I/O完成信息，直接调用处理对象的处理函数
*/
typedef struct _OverLappedEx
{
	OVERLAPPED overLapped;
	IO_OPER    oper;
	DWORD      flags;
	WSABUF     wbuf;
	CHAR      data[MAX_BUF_LEN];
	IAsyncIO*  pAsyncIO;   ///< 当前I/O完成信息的处理对象

	_OverLappedEx(){}      ///< 默认构造
	_OverLappedEx(IAsyncIO* _pAsyncIO, IO_OPER _oper)
		:pAsyncIO(_pAsyncIO)
		,oper(_oper)
		,flags(0)
	{
		pAsyncIO->AddRef();///< 注意每次复制对象，添加引用次数
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

///< 监听 单I/O数据
typedef struct _Accept_OverLappedEx : public _OverLappedEx
{
	SOCKET       sAcceptSocket;     ///< Accepth或者AcceptEx使用的客户连接
	SOCKADDR_IN  RemoteSockAddr;    ///< 远程连接的地址信息

	_Accept_OverLappedEx(IAsyncIO* _pAsyncIO)
		:_OverLappedEx(_pAsyncIO, IO_ACCEPT)
		,sAcceptSocket(INVALID_SOCKET)
	{}
}PER_ACCEPT_CONTEXT, *LPPER_ACCEPT_CONTEXT;

/////< 单句柄数据，绑定IOCP时的Key值
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
//	std::list<PER_IO_DATA*> lstReusePerIOData;  ///< 循环使用的套接字
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

