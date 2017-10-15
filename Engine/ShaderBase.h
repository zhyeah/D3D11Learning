#ifndef _SHADERBASE_H_
#define _SHADERBASE_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class ModelBase;

class ShaderBase 
{
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	ShaderBase();
	ShaderBase(const ShaderBase&);
	~ShaderBase();

	virtual bool Initialize(ID3D11Device*, HWND, WCHAR*, LPCSTR, WCHAR*, LPCSTR);
	virtual void Shutdown();
	virtual bool Render(ID3D11DeviceContext*, ModelBase*);

	// For setting index count.
	void SetIndexCount(int);
	void SetIndexOffset(int);

	void SetWorldMatrix(const XMMATRIX&);
	void SetViewMatrix(const XMMATRIX&);
	void SetProjectMatrix(const XMMATRIX&);

protected:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, LPCSTR, WCHAR*, LPCSTR);
	virtual void ShutdownShader();
	virtual void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	virtual bool InitializeConstantBuffer(ID3D11Device* device);
	virtual bool InitializeSampleState(ID3D11Device* device);
	virtual void SetVertextInputDesc(D3D11_INPUT_ELEMENT_DESC**, unsigned int&);

	virtual bool SetShaderParameters(ID3D11DeviceContext*, ModelBase*);
	virtual void RenderShader(ID3D11DeviceContext*, ModelBase*);

	XMMATRIX m_world;
	XMMATRIX m_view;
	XMMATRIX m_project;

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_cbPerObject;

	ID3D11SamplerState* m_sampleState;

	int m_indexCount;
	int m_indexOffset;
};

#endif // ! _SHADERBASE_H_

