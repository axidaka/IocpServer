#ifndef _RET_STATUS_H_
#define _RET_STATUS_H_


typedef unsigned long long RET_STATUS;
typedef unsigned long HARDWARE_HANDLE_STATUS;

// ������ʹ��64λ�ֽڣ���32λΪӲ�����صĴ�����:
//
//    1     2    3    4    5    6    7   8
//  +----+----+----+----+----+----+----+----+
//  | �������� |    |   |      ������       |
//  +----+----+----+----+----+----+----+----+
// 
// 1,2�����ֽڱ�ʾ�������ͣ����������󡢻�о����VWAS�ڲ�����
// 3,4�����ֽ��Զ���
// 5,6,7,8�ĸ��ֽڱ�ʾӲ��������

/*
    1,2�����ֽ�	         ��������
	0x1	                 VWAS������
	0x2	                 ������SDK����
	0x3	                 ��оSDK����
	0x4	                 ��Դ�������

	3,4�����ֽڣ��ɸ�ģ�����ж��壬����˵���£�

	3,4�����ֽ�	         ˵��
	0x1	                 ������SDK�ڲ�����������δ����
	0x2	                 Ark3000����������ִ�з��صĴ�����
	0x3	                 xlan����������ִ�з��صĴ�����
	0x4	                 Apϵ�д���������ִ�з��صĴ�����
	0x5	                 ������SDK�ڲ�����ϵͳ�������صĴ����룬��getlasterror/wsagetlasterror�ķ���ֵ
*/

#define MAKEERRORCODE64(usErrFrom, usErrCustom, uiErrCode) (((RET_STATUS)((unsigned short)(usErrFrom))<<48)|((RET_STATUS)((unsigned short)(usErrCustom))<<32)|((unsigned int)(uiErrCode)))
#define GETERRFROMECODE(errCode64bit) ((unsigned short)(((RET_STATUS)(errCode64bit))>>48))
#define GETERRCUSTOMCODE(errCode64bit) ((unsigned short)(((RET_STATUS)(errCode64bit))>>32))
#define GETERRCODE32(errCode64bit) ((unsigned int)((RET_STATUS)(errCode64bit)))

//////////////////////
// ��������
//

#define ERR_FROM_VWAS			               (0x1)
#define ERR_FROM_PROCESSOR				       (0x2)
#define ERR_FROM_CUBE				           (0x3)
#define ERR_FROM_RESOUCE				       (0x4)
#define ERR_FROM_VWASCLIENTDLL	               (0x5)
#define ERR_FROM_NETSOCK     	               (0x6)
#define ERR_FROM_VASYNC						   (0x6)
#define ERR_FROM_VPORT      	               (0x7)
#define ERR_FROM_SPATH      	               (0x8)
#define ERR_FROM_DESKTOP					   (0x9)

/////////////////////////////////////////

#define ERR_32CODE_SUCCESS		                (0x0) //����ִ�гɹ�
#define ERR_32CODE_SUCCESS_ERROR		        (0x1) //�ɹ�,���д�����

//��ȫ�ɹ��������������
#define IsSuccessed(errCode64bit) (ERR_32CODE_SUCCESS == GETERRCODE32(errCode64bit))

//�ɹ����������������
#define IsWeakSuccessed(errCode64bit) (ERR_32CODE_SUCCESS == GETERRCODE32(errCode64bit) || ERR_32CODE_SUCCESS_ERROR == GETERRCODE32(errCode64bit))

//��ȫʧ��
#define IsFailed(errCode64bit) (ERR_32CODE_SUCCESS != GETERRCODE32(errCode64bit))

#endif