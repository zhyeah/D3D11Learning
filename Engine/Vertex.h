#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

typedef struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texture;
} Vertex;

#endif // !_VERTEX_H_

