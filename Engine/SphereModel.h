#ifndef _SPHEREMODEL_H_
#define _SPHEREMODEL_H_

#include <vector>
#include "ModelBase.h"
#include "NormalShader.h"
using namespace std;

class SphereModel : public ModelBase
{
private:
	typedef struct MeshType 
	{
		vector<Vertex> vertices;
		vector<UINT> indices;
	} Mesh;

public:
	SphereModel();
	SphereModel(const SphereModel&);
	~SphereModel();

	virtual void Render(ID3D11DeviceContext*, NormalShader*);

	float GetRadius();
	bool Initialize(ID3D11Device*, float, UINT, UINT, LPCWSTR);

protected:
	virtual bool InitializeBuffers(ID3D11Device*);

	float m_radius;
	UINT m_stackCount, m_sliceCount;
};

#endif