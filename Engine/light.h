#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>
using namespace DirectX;

struct DirectionalLight
{
	XMVECTOR ambient;
	XMVECTOR diffuse;
	XMVECTOR specular;
	XMVECTOR direction;
};

struct PointLight
{
	XMVECTOR ambient;
	XMVECTOR diffuse;
	XMVECTOR specular;

	XMFLOAT3 position;
	float range;

	XMVECTOR att;
};

struct SpotLight
{
	XMVECTOR ambient;
	XMVECTOR diffuse;
	XMVECTOR specular;

	XMFLOAT3 position;
	float range;

	XMFLOAT3 direction;
	float spot;

	XMVECTOR att;
};

#endif // !_LIGHT_H_

