#include "RectangleModel.h"

RectangleModel::RectangleModel()
{
}

RectangleModel::RectangleModel(const RectangleModel &rectangleModel): ModelBase(rectangleModel)
{
}

RectangleModel::~RectangleModel()
{
}

void RectangleModel::Render(ID3D11DeviceContext* deviceContext, NormalShader* shader)
{
	RenderBuffers(deviceContext);

	// Put the constant data into shader.
	shader->SetIndexCount(this->GetIndexCount());
	shader->SetWorldMatrix(GetWorldMatrix());

	shader->Render(deviceContext, this);
}

bool RectangleModel::Initialize(ID3D11Device* device, float width, float height)
{
	m_width = width;
	m_height = height;

	return ModelBase::Initialize(device);
}

bool RectangleModel::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the number of indices in the index array.
	m_indexCount = 6;

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

	// left top
	m_vertices[0].position = XMFLOAT3(-m_width / 2, m_height / 2, 0);
	m_vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_vertices[0].texture = XMFLOAT2(1.0f, 0.0f);

	// right top
	m_vertices[1].position = XMFLOAT3(m_width / 2, m_height / 2, 0);
	m_vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_vertices[1].texture = XMFLOAT2(0.0f, 0.0f);

	// right bottom
	m_vertices[2].position = XMFLOAT3(m_width / 2, -m_height / 2, 0);
	m_vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// left bottom
	m_vertices[3].position = XMFLOAT3(-m_width / 2, -m_height / 2, 0);
	m_vertices[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	// left top triangle.
	m_indices[0] = 0; m_indices[1] = 1; m_indices[2] = 3;
	// right bottom triangle.
	m_indices[3] = 1; m_indices[4] = 2; m_indices[5] = 3;

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
