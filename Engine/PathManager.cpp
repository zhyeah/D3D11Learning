#include "PathManager.h"

LPWCH PathManager::GetShaderFilePath(LPCWSTR shaderFileName)
{
	WCHAR* buffer = new WCHAR[256];
	_wgetcwd(buffer, 256);

	wcscat(buffer, L"\\shader\\");
	wcscat(buffer, shaderFileName);

	return buffer;
}

LPWCH PathManager::GetTextureFilePath(LPCWSTR textureFileName)
{
	WCHAR* buffer = new WCHAR[256];
	_wgetcwd(buffer, 256);

	wcscat(buffer, L"\\texture\\");
	wcscat(buffer, textureFileName);

	return buffer;
}