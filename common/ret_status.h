#ifndef _RET_STATUS_H_
#define _RET_STATUS_H_


typedef unsigned long long RET_STATUS;
typedef unsigned long HARDWARE_HANDLE_STATUS;

// 错误码使用64位字节，低32位为硬件返回的错误码:
//
//    1     2    3    4    5    6    7   8
//  +----+----+----+----+----+----+----+----+
//  | 错误类型 |    |   |      错误码       |
//  +----+----+----+----+----+----+----+----+
// 
// 1,2两个字节表示错误类型（处理器错误、机芯错误、VWAS内部错误）
// 3,4两个字节自定义
// 5,6,7,8四个字节表示硬件错误码

/*
    1,2两个字节	         错误类型
	0x1	                 VWAS错误码
	0x2	                 处理器SDK错误
	0x3	                 机芯SDK错误
	0x4	                 资源服务错误

	3,4两个字节，由各模块自行定义，举例说明下：

	3,4两个字节	         说明
	0x1	                 处理器SDK内部错误，如连接未建立
	0x2	                 Ark3000处理器命令执行返回的错误码
	0x3	                 xlan处理器命令执行返回的错误码
	0x4	                 Ap系列处理器命令执行返回的错误码
	0x5	                 处理器SDK内部调用系统函数返回的错误码，即getlasterror/wsagetlasterror的返回值
*/

#define MAKEERRORCODE64(usErrFrom, usErrCustom, uiErrCode) (((RET_STATUS)((unsigned short)(usErrFrom))<<48)|((RET_STATUS)((unsigned short)(usErrCustom))<<32)|((unsigned int)(uiErrCode)))
#define GETERRFROMECODE(errCode64bit) ((unsigned short)(((RET_STATUS)(errCode64bit))>>48))
#define GETERRCUSTOMCODE(errCode64bit) ((unsigned short)(((RET_STATUS)(errCode64bit))>>32))
#define GETERRCODE32(errCode64bit) ((unsigned int)((RET_STATUS)(errCode64bit)))

//////////////////////
// 错误类型
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

#define ERR_32CODE_SUCCESS		                (0x0) //命令执行成功
#define ERR_32CODE_SUCCESS_ERROR		        (0x1) //成功,但有错误发生

//完全成功，不允许错误发生
#define IsSuccessed(errCode64bit) (ERR_32CODE_SUCCESS == GETERRCODE32(errCode64bit))

//成功，但是允许错误发生
#define IsWeakSuccessed(errCode64bit) (ERR_32CODE_SUCCESS == GETERRCODE32(errCode64bit) || ERR_32CODE_SUCCESS_ERROR == GETERRCODE32(errCode64bit))

//完全失败
#define IsFailed(errCode64bit) (ERR_32CODE_SUCCESS != GETERRCODE32(errCode64bit))

#endif