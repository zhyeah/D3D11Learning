#ifndef _MODELBASE_H_
#define _MODELBASE_H_

#include <d3d11.h>
#include <directxmath.h>
#include "Vertex.h"
#include "Material.h"
#include "TextureBase.h"
#include "ShaderBase.h"
using namespace DirectX;

class ModelBase 
{
public:
	ModelBase();
	ModelBase(const ModelBase&);
	~ModelBase();

	virtual bool Initialize(ID3D11Device*);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*, ShaderBase*);
	virtual void UpdateModel(float t);

	// Load or get texture view
	virtual bool LoadTexture(ID3D11Device*, LPCWSTR);
	ID3D11ShaderResourceView* GetTexture();
	void SetTexture(ID3D11ShaderResourceView*);

	// Get or set material
	Material GetMaterial();
	void SetMaterial(const Material&);
	void SetMaterial(const XMVECTOR&, const XMVECTOR&, const XMVECTOR&, const XMVECTOR&);

	// Get or set position
	XMVECTOR GetPostion();
	void SetPosition(const XMVECTOR&);

	// Set rotation.
	void SetRotation(float pitch, float yaw, float roll);

	// Get model world matrix.
	XMMATRIX GetWorldMatrix();

	// Get the length of model index buffer
	int GetIndexCount();

	// Get or set whether to use texture in shader.
	void SetUseTexture(bool useTexture);
	bool GetUseTexture();

protected:
	virtual bool InitializeBuffers(ID3D11Device*);
	virtual void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext*);
	virtual void UpdateBuffers(ID3D11DeviceContext* deviceContext);


	virtual void ReleaseTexture();

	bool m_dirty; // Used to indicate whether to update buffer;

	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer; // The vertex and index buffer.
	TextureBase* m_texture; // The class that help to load texture.
	
	Material m_material; // The material of this model.
	XMVECTOR m_position; // The position of this model.
	float m_pitch, m_yaw, m_roll; // The rotation of this model.

	Vertex* m_vertices; // vertices data.
	unsigned long* m_indices; // index data.
	int m_vertexCount, m_indexCount; // Record the count of vertex and index.
	bool m_useTexture; //Denote whether use texture.
};

#endif // !_MODELBASE_H_

