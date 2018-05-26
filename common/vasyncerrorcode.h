#ifndef _VASYNCERRORCODE_H_
#define _VASYNCERRORCODE_H_

#include "ret_status.h"

#define ERR_CUSTOM_VASYNC_LOGIC		                (0x1) //32CODEֵ���Զ���Ĵ�����
#define ERR_CUSTOM_VASYNC_API  		                (0x2) //32CODEֵ��GetLastError()���صĴ�����

#define ERR_64CODE_SYSTEM_API(nLastError) \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_API, (nLastError))

#define ERR_64CODE_VASYNC_SUCCESS \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_LOGIC, ERR_32CODE_SUCCESS)

#define ERR_32CODE_VASYNC_UNKNOWN		                   (0xFFFFFFFE)
#define ERR_64CODE_VASYNC_UNKNOWN \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_LOGIC, ERR_32CODE_VASYNC_UNKNOWN)

#define ERR_32CODE_VASYNC_NOTINITNET		                (0x0001)
#define ERR_64CODE_VASYNC_NOTINITNET \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_LOGIC, ERR_32CODE_VASYNC_NOTINITNET)

#define ERR_32CODE_VASYNC_NOMEMORY							(0x0002)
#define ERR_64CODE_VASYNC_NOMEMORY \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_LOGIC, ERR_32CODE_VASYNC_NOMEMORY)

#define ERR_32CODE_VASYNC_PARAMERROR		                (0x0003)
#define ERR_64CODE_VASYNC_PARAMERROR \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_LOGIC, ERR_32CODE_VASYNC_PARAMERROR)

#define ERR_32CODE_VASYNC_CONNECTIONCLOSED		            (0x0004)
#define ERR_64CODE_VASYNC_CONNECTIONCLOSED \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_LOGIC, ERR_32CODE_VASYNC_CONNECTIONCLOSED)

#define ERR_32CODE_VASYNC_INVALIDSOCK					    (0x0005)   ///< ����ЧSocket�ϲ���
#define ERR_64CODE_VASYNC_INVALIDSOCK \
	MAKEERRORCODE64(ERR_FROM_VASYNC, ERR_CUSTOM_VASYNC_LOGIC, ERR_32CODE_VASYNC_INVALIDSOCK)

#endif //_VASYNCERRORCODE_H_