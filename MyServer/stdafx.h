// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
//#include "DetectMemoryLeak.h"      ///< ����ڴ�й¶

#include <assert.h>
#include "glog/logging.h"

#ifdef _DEBUG
	#pragma comment(lib,"libglogd.lib")  
#else
	#pragma comment(lib,"libglog.lib")  
#endif
