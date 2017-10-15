#include "StringHelper.h"

bool StringHelper::IsNullOrEmpty(WCHAR* str)
{
	return str == NULL || wcscmp(str, L"");
}