#ifndef _SURFACEMODEL_H_
#define _SURFACEMODEL_H_

#include "ModelBase.h"

class SurfaceModel : public ModelBase
{
public:
	SurfaceModel();
	SurfaceModel(const SurfaceModel&);
	~SurfaceModel();

	bool Initialize(ID3D11Device*, UINT, float, float);

protected:
	virtual bool InitializeBuffers(ID3D11Device*);

	float m_width, m_height;
	UINT m_gridCount, m_pointCount;
};

#endif // ! _SURFACEMODEL_H_