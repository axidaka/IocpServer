#pragma once

#include <algorithm>
#include <iterator>
#include <functional>
#include <iostream>
#include <string>			///for stl::string
#include <list>				///for stl::list
#include <queue>
#include <map>			///for stl::map
#include <set>				///for stl::set
#include <tchar.h>		///for TCHAR
#include <stack>			///for stl::stack
#include <vector>		///for stl::vector
#include <sstream>
#include <memory>
#include <fstream>
#include <iomanip>   ///for setfill, setw

#include <tchar.h>


#ifdef _UNICODE

typedef std::basic_ostream<wchar_t, std::char_traits<wchar_t>> t_basic_ostream;
typedef std::wstringstream tstringstream;
typedef std::wistringstream tistringstream;
typedef std::wostringstream tostringstream;
typedef std::wstring tstring;
typedef wchar_t tchar;
typedef std::wfstream tfstream;
typedef std::wifstream tifstream;
typedef std::wofstream tofstream;

#define tcout std::wcout

#else

typedef std::basic_ostream<char, std::char_traits<char>> t_basic_ostream;
typedef std::stringstream tstringstream;
typedef std::istringstream tistringstream;
typedef std::ostringstream tostringstream;
typedef std::string tstring;
typedef char tchar;
typedef std::fstream tfstream;
typedef std::ifstream tifstream;
typedef std::ofstream tofstream;

#define tcout std::cout

#endif

#define ENDL std::endl

#define std_auto_ptr std::auto_ptr
#define std_shared_ptr std::tr1::shared_ptr
