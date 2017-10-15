#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <directxmath.h>
using namespace DirectX;

struct Material
{
	XMVECTOR ambient;
	XMVECTOR diffuse;
	XMVECTOR specular;
	XMVECTOR reflect;
};

#endif // !_MATERIAL_H_

