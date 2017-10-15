#ifndef _STRINGHELPER_H_
#define _STRINGHELPER_H_

#include <d3d11.h>
#include <direct.h>
#include <string.h>

class StringHelper
{
public:
	static bool IsNullOrEmpty(WCHAR* str);
};

#endif
