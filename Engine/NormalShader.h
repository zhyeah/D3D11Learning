#ifndef _NORMALSHADER_H_
#define _NORMALSHADER_H_

#include "ShaderBase.h"
#include "light.h"
#include "Material.h"
#include "ModelBase.h"
#include "MathHelper.h"
using namespace DirectX;

class NormalShader : public ShaderBase
{
private:
	struct CBPerFrame 
	{
		DirectionalLight directionLight;
		PointLight pointLight;
		SpotLight spotLight;
		XMVECTOR eyePosition;
	};

	struct CBPerObject 
	{
		XMMATRIX world;
		XMMATRIX inverseTransposeWorld;
		XMMATRIX worldViewProj;
		Material material;
		bool useTexture;
	};

public:
	NormalShader();
	NormalShader(const NormalShader&);
	~NormalShader();

	void SetDirectionalLight(DirectionalLight&);
	void SetPointLight(PointLight&);
	void SetSpotLight(SpotLight&);
	void SetEyePosition(XMFLOAT3&);
	void SetMaterial(Material&);

protected:
	virtual bool InitializeConstantBuffer(ID3D11Device*);
	virtual bool InitializeSampleState(ID3D11Device* device);
	virtual void SetVertextInputDesc(D3D11_INPUT_ELEMENT_DESC**, unsigned int&);
	virtual bool SetShaderParameters(ID3D11DeviceContext*, ModelBase*);
	virtual void ShutdownShader();

	ID3D11Buffer* m_cbPerFrame;

	DirectionalLight m_directionalLight;
	PointLight m_pointLight;
	SpotLight m_spotLight;
	XMFLOAT3 m_eyePosition;
	Material m_material;
};

#endif // !_NORMALSHADER_H_

