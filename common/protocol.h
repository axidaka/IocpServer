#pragma once

///< �۲���ģʽ�е�֪ͨ��Ϣö������
typedef enum 
{
	NOTISFY_MSG_SERVER_NEWCLIENT        = 0x01, ///< ���������Զ�̿ͻ�����
	NOTISFY_MSG_SERVER_DISCONNECTCLIENT = 0x02, ///< ����������Ͽ�Զ�̿ͻ�����
	NOTISFY_MSG_CLIENT_NEWCLIENT        = 0x03, ///< �ͻ�������Զ�̿ͻ�����
	NOTISFY_MSG_CLIENT_DISCONNECTCLIENT = 0x04, ///< �ͻ��������Ͽ�����
}NOTISY_MSG_TYPE;

#define  IOCP_EXIT_FLAG  0xFFFFFFFF
#define  THREADS_NUM     4
#define  ACCEPT_NUM      10