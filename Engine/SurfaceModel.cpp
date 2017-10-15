#include "SurfaceModel.h"

SurfaceModel::SurfaceModel()
{
	m_gridCount = 0;
	m_pointCount = 0;
	m_width = 0;
	m_height = 0;
}

SurfaceModel::SurfaceModel(const SurfaceModel& surfaceModel): ModelBase(surfaceModel)
{
}

SurfaceModel::~SurfaceModel()
{
}

bool SurfaceModel::Initialize(ID3D11Device* device, UINT gridCount, float width, float height)
{
	m_gridCount = gridCount;
	m_pointCount = gridCount + 1;
	m_width = width;
	m_height = height;

	return ModelBase::Initialize(device);
}

bool SurfaceModel::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set vertex count.
	m_vertexCount = m_pointCount * m_pointCount;

	// Set index count.
	m_indexCount = m_gridCount * m_gridCount * 6;

	// Create the vertex array.
	m_vertices = new Vertex[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// Create the index array.
	m_indices = new unsigned long[m_indexCount];
	if (!m_indices)
	{
		return false;
	}

	// Make vertex buffer data.
	for (int i = 0; i < m_pointCount; i++)
	{
		for (int j = 0; j < m_pointCount; j++)
		{
			int index = i * m_pointCount + j;

			m_vertices[index].position = XMFLOAT3(
			    (i - m_pointCount / 2.0f) * m_width / m_pointCount,
				0.0f,
				(j - m_pointCount / 2.0f) * m_height / m_pointCount);

			m_vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		}
	}

	// Make index buffer data.
	int indice_index = 0;
	for (int i = 0; i < m_gridCount; i++)
	{
		for (int j = 0; j < m_gridCount; j++)
		{
			int index = i * m_pointCount + j;

			// up triangle.
			m_indices[indice_index++] = index;
			m_indices[indice_index++] = index + 1;
			m_indices[indice_index++] = index + m_pointCount;

			// down triangle.
			m_indices[indice_index++] = index + m_pointCount;
			m_indices[indice_index++] = index + 1;
			m_indices[indice_index++] = index + 1 + m_pointCount;
		}
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(int) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}
