#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// Include system head file.
#include "d3dclass.h"
#include "Camera.h"
#include "SphereModel.h"
#include "RectangleModel.h"
#include "OceanModel.h"
#include "NormalShader.h"
#include "PathManager.h"
#include "light.h"

class GraphicsClass 
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float dt);

private:
	void InitializeLight(DirectionalLight&, PointLight&, SpotLight&);

	float m_totalTimeElapse;

	D3DClass* m_direct3D;
	Camera* m_camera;
	SphereModel* m_model;
	RectangleModel* m_mirror;
	OceanModel* m_ocean;
	NormalShader* m_shader;

	DirectionalLight m_directionalLight;
	PointLight m_pointLight;
	SpotLight m_spotLight;

	bool Render(float dt);
};

#endif