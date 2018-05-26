// MyServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stldefine.h"
#include "AsyncListen.h"
#include "ThreadPoolSelfCreate.h"
#include "NetworkInterface.h"
#include "NetServer.h"
#include "DetectMemoryLeak.h"      ///< ����ڴ�й¶

int _tmain(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	google::InitGoogleLogging(argv[0]);  
	google::LogToStderr();
	FLAGS_colorlogtostderr = true;

	NetworkInterface::InitNetwork();

	CNetServer netServer;
	netServer.InitServer();

	netServer.StartListen("0.0.0.0", 12345);

	char cinput;
	while(std::cin>>cinput)
	{
		if (cinput == 'q' || cinput == 'Q')
		{
			break;
		}
	}

	netServer.UninitServer();

	NetworkInterface::DeinitNetwork();

	google::ShutdownGoogleLogging();

	
	return 0;
}

