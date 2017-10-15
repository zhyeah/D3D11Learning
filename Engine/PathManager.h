#ifndef _PATHMANAGER_H_
#define _PATHMANAGER_H_

#include <d3d11.h>
#include <direct.h>
#include <string.h>

class PathManager 
{
public:
	static LPWCH GetShaderFilePath(LPCWSTR);
	static LPWCH GetTextureFilePath(LPCWSTR);
};

#endif // ! _PATHMANAGER_H_

