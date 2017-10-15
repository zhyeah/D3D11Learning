#ifndef _RECTANGLEMODEL_H_
#define _RECTANGLEMODEL_H_

#include "ModelBase.h"
#include "NormalShader.h"

class RectangleModel: public ModelBase
{
public:
	RectangleModel();
	RectangleModel(const RectangleModel&);
	~RectangleModel();

	virtual void Render(ID3D11DeviceContext*, NormalShader*);

	bool Initialize(ID3D11Device*, float, float);

protected:
	virtual bool InitializeBuffers(ID3D11Device*);

	float m_height, m_width;
};

#endif