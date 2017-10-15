#include "SphereModel.h"

SphereModel::SphereModel()
{
}

SphereModel::SphereModel(const SphereModel& sphere) : ModelBase(sphere)
{
}

SphereModel::~SphereModel()
{
}

void SphereModel::Render(ID3D11DeviceContext* deviceContext, NormalShader* shader)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	// Put the constant data into shader.
	shader->SetIndexCount(this->GetIndexCount());
	shader->SetWorldMatrix(GetWorldMatrix());

	shader->Render(deviceContext, this);
}

bool SphereModel::Initialize(ID3D11Device* device, float radius, UINT stackCount, UINT sliceCount, LPCWSTR textureFilename)
{
	m_radius = radius;
	m_stackCount = stackCount;
	m_sliceCount = sliceCount;

	// Load the texture for this model.
	HRESULT result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	// Directly invoke the initialize function of base class
	return ModelBase::Initialize(device);
}

float SphereModel::GetRadius() {
	return m_radius;
}

bool SphereModel::InitializeBuffers(ID3D11Device* device)
{
	Mesh meshData;
	HRESULT result;

	// Construct top vertex of sphere.
	Vertex topVertex;
	topVertex.position = XMFLOAT3(0, m_radius, 0);
	topVertex.normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	topVertex.texture = XMFLOAT2(0.0f, 0.0f);
	meshData.vertices.push_back(topVertex);

	float phiStep = XM_PI / m_stackCount;
	float thetaStep = 2 * XM_PI / m_sliceCount;

	// Construct body vertex of sphere.
	for (UINT i = 1; i < m_stackCount; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= m_sliceCount; j++) 
		{
			Vertex v;
			float theta = j * thetaStep;

			// Calculate postion of this vertex.
			v.position = XMFLOAT3(m_radius * sinf(phi) * cosf(theta), 
				m_radius * cosf(phi), 
				m_radius * sinf(phi) * sinf(theta));

			// Give its normal vector.
			XMVECTOR norm = XMLoadFloat3(&v.position);
			XMStoreFloat3(&v.normal, norm);

			// Give texture coordinate.
			float U = theta / XM_2PI;
			float V = phi / XM_PI;
			v.texture = XMFLOAT2(U, V);

			meshData.vertices.push_back(v);
		}
	}

	// Construct bottom vertex
	Vertex bottomVertex;
	bottomVertex.position = XMFLOAT3(0, -m_radius, 0);
	bottomVertex.normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	bottomVertex.texture = XMFLOAT2(0.0f, 1.0f);
	meshData.vertices.push_back(bottomVertex);

	// Build top index
	for (UINT i = 0; i < m_sliceCount; i++) 
	{
		meshData.indices.push_back(0);
		meshData.indices.push_back(1 + (i+1));
		meshData.indices.push_back(1 + i);
	}

	// Build body index
	UINT baseIndex = 1;
	for (UINT i = 0; i < m_stackCount - 2; i++)
	{
		for (UINT j = 0; j < m_sliceCount; j++)
		{
			meshData.indices.push_back(baseIndex + i * (m_sliceCount + 1) + j);
			meshData.indices.push_back(baseIndex + i * (m_sliceCount + 1) + (j + 1));
			meshData.indices.push_back(baseIndex + (i + 1) * (m_sliceCount + 1) + j);

			meshData.indices.push_back(baseIndex + (i + 1) * (m_sliceCount + 1) + j);
			meshData.indices.push_back(baseIndex + i * (m_sliceCount + 1) + (j + 1));
			meshData.indices.push_back(baseIndex + (i + 1) * (m_sliceCount + 1) + (j + 1));
		}
	}

	// Build bottom index
	UINT lastIndex = meshData.vertices.size() - 1;
	baseIndex = meshData.vertices.size() - m_sliceCount - 1;
	for (UINT i = 0; i < m_sliceCount; i++)
	{
		meshData.indices.push_back(lastIndex);
		meshData.indices.push_back(baseIndex + i);
		meshData.indices.push_back(baseIndex + (i + 1));
	}

	m_vertexCount = meshData.vertices.size();
	m_indexCount = meshData.indices.size();

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &(meshData.vertices[0]);
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = &(meshData.indices[0]);
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}