#include "ModelBase.h"

ModelBase::ModelBase() 
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_texture = NULL;
	
	m_vertices = NULL;
	m_indices = NULL;

	m_position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	m_pitch = 0.0f, m_yaw = 0.0f, m_roll = 0.0f;
	m_useTexture = false;
	m_dirty = false;
}

ModelBase::ModelBase(const ModelBase&) 
{
}

ModelBase::~ModelBase() 
{
}

bool ModelBase::Initialize(ID3D11Device* device)
{
	bool result;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelBase::Shutdown() 
{
	ReleaseTexture();
	ShutdownBuffers();
}

void ModelBase::Render(ID3D11DeviceContext* deviceContext, ShaderBase* shader)
{
	// If there is some update of model, update the buffers.
	if (m_dirty)
	{
		UpdateBuffers(deviceContext);
		m_dirty = false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	// Put the constant data into shader.
	//shader->SetMaterial(material);
	shader->SetIndexCount(this->GetIndexCount());
	shader->SetWorldMatrix(GetWorldMatrix());

	shader->Render(deviceContext, this);

	return;
}

void ModelBase::UpdateModel(float t)
{
	m_dirty = true;
}

int ModelBase::GetIndexCount()
{
	return m_indexCount;
}

void ModelBase::SetUseTexture(bool useTexture)
{
	m_useTexture = useTexture;
}

bool ModelBase::GetUseTexture()
{
	return m_useTexture;
}

ID3D11ShaderResourceView * ModelBase::GetTexture()
{
	return m_texture->GetTexture();
}

void ModelBase::SetTexture(ID3D11ShaderResourceView* shaderResourceView)
{
	if (m_texture == NULL)
	{
		m_texture = new TextureBase();
	}

	m_texture->SetTexture(shaderResourceView);
	m_useTexture = true;
}

Material ModelBase::GetMaterial()
{
	return m_material;
}

void ModelBase::SetMaterial(const Material& material)
{
	m_material = material;
}

void ModelBase::SetMaterial(const XMVECTOR& ambient, const XMVECTOR& diffuse, const XMVECTOR& specular, const XMVECTOR& reflect)
{
	m_material.ambient = ambient;
	m_material.diffuse = diffuse;
	m_material.specular = specular;
	m_material.reflect = reflect;
}

XMVECTOR ModelBase::GetPostion()
{
	return m_position;
}

void ModelBase::SetPosition(const XMVECTOR& position)
{
	m_position = position;
}

void ModelBase::SetRotation(float pitch, float yaw, float roll)
{
	m_pitch = pitch;
	m_yaw = yaw;
	m_roll = roll;
}

XMMATRIX ModelBase::GetWorldMatrix()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(m_position);
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll);

	return rotate * translation;
}

bool ModelBase::InitializeBuffers(ID3D11Device* device)
{
	return true;
}

void ModelBase::ShutdownBuffers()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}

	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = NULL;
	}

	if (m_indices)
	{
		delete[] m_indices;
		m_indices = NULL;
	}
}

void ModelBase::ReleaseTexture()
{
	// Release the texture object.
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}

	return;
}

void ModelBase::RenderBuffers(ID3D11DeviceContext* deviceContext) 
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ModelBase::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
}

bool ModelBase::LoadTexture(ID3D11Device* device, LPCWSTR fileName)
{
	bool result;

	// Create the texture object.
	m_texture = new TextureBase;
	if (!m_texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_texture->Initialize(device, fileName);
	if (!result)
	{
		return false;
	}

	m_useTexture = true;

	return true;
}
