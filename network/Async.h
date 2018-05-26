#ifndef _ASYNC_H_
#define _ASYNC_H_

#include "ret_status.h"
#include "vasyncerrorcode.h"

typedef unsigned int UINT;
typedef int    BOOL;
typedef unsigned long DWORD;

/*
@brief  ���ô����ӿ��࣬�����ô���Ϊ0ʱ�ͷŶ���
		�����ڴ�й¶����������������ָ��
@author zhengqingsong
@date   2013/10/30
*/

class IRef
{
public:

	virtual void AddRef() = 0;
	virtual void Release() = 0;

protected:

	virtual ~IRef(){};
};

/*
@brief �첽IO�ӿ���
@duty  1.ʵ�����ü�������������Ϊ0ʱ�ͷŶ���
	   2.�����첽����֪ͨ����
@author  zhqsong
@date    2013/11/1
*/
class IAsyncIO : public IRef
{

public:

	IAsyncIO(void);
	virtual ~IAsyncIO(void);

	///< Override From IRef
	virtual void AddRef();
	virtual void Release();


	///< Overridable Notification Functions
	virtual bool OnAsyncExcute(BOOL bRet, DWORD dwBytesTransfered, void* pParam) = 0;

private:

	long m_lRefCount;    ///< ���ô���
};

/*
@brief   �첽Socket�ӿ���
		����������麯��
@author  zhqsong
@date    2013/11/1
*/

class IAsyncSocket : public IAsyncIO
{
public:

	IAsyncSocket(void);
	virtual ~IAsyncSocket(void);

	///< �������ر��׽���
	virtual RET_STATUS OpenSocket(const char* pStrIP, const UINT nPort, const bool bTcp = true) = 0;
	virtual RET_STATUS OpenSocket(const wchar_t* pWStrIP, const UINT nPort, const bool bTcp = true) = 0;
	virtual RET_STATUS CloseSocket() = 0;
};

#endif //_ASYNC_H_

