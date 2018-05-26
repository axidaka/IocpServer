#pragma once 

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)   // 添加可以提示内存泄露具体代码位置



