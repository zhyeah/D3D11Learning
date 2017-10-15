#include "OceanModel.h"

OceanModel::OceanModel()
{
	m_amp = 0.0f;
	m_g = 9.8f;
	m_windSpeed.x = 0.0f;
	m_windSpeed.y = 0.0f;

	m_vertexTildeArray = NULL;
	m_vertexTildeConjArray = NULL;

	m_fft = NULL;
	m_htilde = NULL;
	m_htilde_dx = NULL;
	m_htilde_dz = NULL;
	m_htilde_slopex = NULL;
	m_htilde_slopez = NULL;

	m_originPosition = NULL;
}

OceanModel::OceanModel(const OceanModel& oceanModel): SurfaceModel(oceanModel)
{
}

OceanModel::~OceanModel()
{
}

void OceanModel::Shutdown()
{
	if (m_vertexTildeArray)
	{
		delete[] m_vertexTildeArray;
		m_vertexTildeArray = NULL;
	}

	if (m_vertexTildeConjArray)
	{
		delete[] m_vertexTildeConjArray;
		m_vertexTildeConjArray = NULL;
	}

	if (m_fft)
	{
		m_fft->Release();
		delete m_fft;
		m_fft = NULL;
	}

	if (m_htilde)
	{
		delete[] m_htilde;
		m_htilde = NULL;
	}

	if (m_htilde_dx)
	{
		delete[] m_htilde_dx;
		m_htilde_dx = NULL;
	}

	if (m_htilde_dz)
	{
		delete[] m_htilde_dz;
		m_htilde_dz = NULL;
	}

	if (m_htilde_slopex)
	{
		delete[] m_htilde_slopex;
	    m_htilde_slopex = NULL;
	}

	if (m_htilde_slopez)
	{
		delete[] m_htilde_slopez;
		m_htilde_slopez = NULL;
	}

	if (m_originPosition)
	{
		delete[] m_originPosition;
		m_originPosition = NULL;
	}

	SurfaceModel::Shutdown();
}

void OceanModel::UpdateModel(float t)
{
	UpdateWavesWithFFT(t);
	m_dirty = true;
}

bool OceanModel::Initialize(ID3D11Device* device, UINT gridCount, float length, float amp, float period, XMFLOAT2 windSpeed)
{
	m_amp = amp;
	m_windSpeed = windSpeed;
	m_period = period;

	m_gridCount = gridCount;
	m_pointCount = gridCount + 1;
	m_width = length;
	m_height = length;

	//Initialize fft tool.
	m_fft = new FFT();
	m_fft->Initialize(m_pointCount);

	//Initialize the array for FFT
	int alen = m_pointCount * m_pointCount;
	m_htilde = new XMFLOAT2[alen];
	m_htilde_slopex = new XMFLOAT2[alen];
	m_htilde_slopez = new XMFLOAT2[alen];
	m_htilde_dx = new XMFLOAT2[alen];
	m_htilde_dz = new XMFLOAT2[alen];

	m_vertexTildeArray = new XMFLOAT2[alen];
	m_vertexTildeConjArray = new XMFLOAT2[alen];

	m_originPosition = new XMFLOAT3[alen];

	return ModelBase::Initialize(device);
}

void OceanModel::UpdateWaves(float t)
{
	float lambda = -1.0;
	int index = 0;
	XMFLOAT2 x;
	XMFLOAT2 d;

	for (int i = 0; i < m_pointCount; i++)
	{
		for (int j = 0; j < m_pointCount; j++)
		{
			index = i * m_pointCount + j;

			x.x = m_vertices[index].position.x; x.y = m_vertices[index].position.z;
			XMFLOAT2 height, displacement;
			XMFLOAT3 normal;

			GetHDN(t, x, height, displacement, normal);

			m_vertices[index].position.y = height.x;
			m_vertices[index].position.x += lambda * displacement.x;
			m_vertices[index].position.z += lambda * displacement.y;

			m_vertices[index].normal = normal;
		}
	}
}

void OceanModel::UpdateWavesWithFFT(float t)
{
	float kx, kz, len, lambda = -1.0f;

	for (int i = 0; i < m_pointCount; i++)
	{
		kx = XM_PI * (2.0f * i - m_pointCount) / m_width;
		for (int j = 0; j < m_pointCount; j++)
		{
			kz = XM_PI * (2.0f * j - m_pointCount) / m_width;
			len = sqrt(kx*kx + kz*kz);
			int index = i * m_pointCount + j;

			m_htilde[index] = GetHeightTilde(t, i, j);
			m_htilde_slopex[index].x = -m_htilde[index].y * kx;
			m_htilde_slopex[index].y = m_htilde[index].x * kx;  // ikx * htilde.
			m_htilde_slopez[index].x = -m_htilde[index].y * kz;
			m_htilde_slopez[index].y = m_htilde[index].x * kz; // ikz * hitilde.

			if (len < 0.000001f) {
				m_htilde_dx[index].x = 0.0f;
				m_htilde_dx[index].y = 0.0f;
				m_htilde_dz[index].x = 0.0f;
				m_htilde_dz[index].y = 0.0f;
			}
			else {
				m_htilde_dx[index].x = -m_htilde[index].y * (-kx / len);
				m_htilde_dx[index].y = m_htilde[index].x * (-kx / len);
				m_htilde_dz[index].x = -m_htilde[index].y * (-kz / len);
				m_htilde_dz[index].y = m_htilde[index].x * (-kz / len);
			}
		}
	}

	for (int i = 0; i < m_pointCount; i++) {
		m_fft->FastFourierTransform(m_htilde, m_htilde, 1, i * m_pointCount);
		m_fft->FastFourierTransform(m_htilde_slopex, m_htilde_slopex, 1, i * m_pointCount);
		m_fft->FastFourierTransform(m_htilde_slopez, m_htilde_slopez, 1, i * m_pointCount);
		m_fft->FastFourierTransform(m_htilde_dx, m_htilde_dx, 1, i * m_pointCount);
		m_fft->FastFourierTransform(m_htilde_dz, m_htilde_dz, 1, i * m_pointCount);
	}

	for (int j = 0; j < m_pointCount; j++) {
		m_fft->FastFourierTransform(m_htilde, m_htilde, m_pointCount, j);
		m_fft->FastFourierTransform(m_htilde_slopex, m_htilde_slopex, m_pointCount, j);
		m_fft->FastFourierTransform(m_htilde_slopez, m_htilde_slopez, m_pointCount, j);
		m_fft->FastFourierTransform(m_htilde_dx, m_htilde_dx, m_pointCount, j);
		m_fft->FastFourierTransform(m_htilde_dz, m_htilde_dz, m_pointCount, j);
	}

	int sign;
	for (int i = 0; i < m_pointCount; i++)
	{
		for (int j = 0; j < m_pointCount; j++)
		{
			int index = i * m_pointCount + j;
			sign = ((i + j) & 1) == 0 ? 1 : -1;
			m_htilde[index].x *= sign; m_htilde[index].y *= sign;

			// Update position;
			m_vertices[index].position.y = m_htilde[index].x;
			m_htilde_dx[index].x *= sign;
			m_htilde_dx[index].y *= sign;
			m_htilde_dz[index].x *= sign;
			m_htilde_dz[index].y *= sign;
			m_vertices[index].position.x = m_originPosition[index].x + m_htilde_dx[index].x * lambda;
			m_vertices[index].position.z = m_originPosition[index].z + m_htilde_dz[index].x * lambda;

			// Update normal;
			m_htilde_slopex[index].x *= sign;
			m_htilde_slopex[index].y *= sign;
			m_htilde_slopez[index].x *= sign;
			m_htilde_slopez[index].y *= sign;
			XMVECTOR nVec = XMVectorSet(-m_htilde_slopex[index].x, 1.0f, m_htilde_slopez[index].x, 0.0f);
			nVec = XMVector3Normalize(nVec);
			m_vertices[index].normal.x = nVec.m128_f32[0];
			m_vertices[index].normal.y = nVec.m128_f32[1];
			m_vertices[index].normal.z = nVec.m128_f32[2];
		}
	}
}

bool OceanModel::InitializeBuffers(ID3D11Device * device)
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
			m_originPosition[index] = m_vertices[index].position;

			m_vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);

			m_vertexTildeArray[index] = GetHeightTilde0(i, j);
			m_vertexTildeConjArray[index] = GetHeightTilde0(-i, -j);
			m_vertexTildeConjArray[index].y = -m_vertexTildeConjArray[index].y;
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

	// Give a default wave state to test.
	UpdateWavesWithFFT(0.0f);

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

void OceanModel::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	Vertex* verticesPtr;
	verticesPtr = (Vertex*)mappedResource.pData;
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(Vertex) * m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);
}

float OceanModel::GetDispersion(int n, int m)
{
	float w0 = 2.0f * XM_PI / m_period;
	float kx = XM_PI * (2.0f * n - m_gridCount) / m_width;
	float kz = XM_PI * (2.0f * m - m_gridCount) / m_width;

	return floor(sqrt(m_g * sqrt(kx*kx + kz*kz)) / w0) * w0;
}

float OceanModel::GetPhilipsSpectrum(int n, int m)
{
	float kx = XM_PI * (2.0f * n - m_gridCount) / m_width;
	float ky = XM_PI * (2.0f * m - m_gridCount) / m_width;

	XMFLOAT2 k = XMFLOAT2(kx, ky);
	XMVECTOR kv = XMLoadFloat2(&k);
	XMVECTOR kLenVector = XMVector2Length(kv);

	if (kLenVector.m128_f32[0] < 0.000001) return 0.0;

	float kLen2 = kLenVector.m128_f32[0] * kLenVector.m128_f32[0];
	float kLen4 = kLen2 * kLen2;

	XMVECTOR kvUnit = XMVector2Normalize(kv);
	XMVECTOR windVec = XMLoadFloat2(&m_windSpeed);
	XMVECTOR windUnit = XMVector2Normalize(windVec);

	XMVECTOR kDotW = XMVector2Dot(kvUnit, windUnit);
	float kDotWValue2 = kDotW.m128_f32[0] * kDotW.m128_f32[0];

	XMVECTOR windLen = XMVector2Length(windVec);

	float L = windLen.m128_f32[0] * windLen.m128_f32[0] / m_g;
	float L2 = L * L;

	float damping = 0.001;
	float l2 = L2 * damping * damping;

	return m_amp * exp(-1.0f / (kLen2 * L2)) / kLen4 * kDotWValue2 * exp(-kLen2 * l2);
}

XMFLOAT2 OceanModel::GetHeightTilde0(int n, int m)
{
	XMFLOAT2 gaussRand = XMFLOAT2(GaussRand(), GaussRand());
	XMVECTOR gaussRandVec = XMLoadFloat2(&gaussRand);
	gaussRandVec = gaussRandVec * sqrt(GetPhilipsSpectrum(n, m) / 2.0f);
	XMFLOAT2 result;
	XMStoreFloat2(&result, gaussRandVec);

	return result;
}

XMFLOAT2 OceanModel::GetHeightTilde(float t, int n, int m)
{
	int index = n * m_pointCount + m;
	
	XMFLOAT4 h0Complex = XMFLOAT4(0.0f, 0.0f, m_vertexTildeArray[index].y, m_vertexTildeArray[index].x);
	XMFLOAT4 h0conjComplex = XMFLOAT4(0.0f, 0.0f, m_vertexTildeConjArray[index].y, m_vertexTildeConjArray[index].x);
	XMVECTOR htilde0Vec = XMLoadFloat4(&h0Complex);
	XMVECTOR htilde0ConjVec = XMLoadFloat4(&h0conjComplex);

	float omega = GetDispersion(n, m) * t;
	float cosOmega = cos(omega);
	float sinOmega = sin(omega);

	XMFLOAT4 c0 = XMFLOAT4(0.0f, 0.0f, sinOmega, cosOmega);
	XMFLOAT4 c1 = XMFLOAT4(0.0f, 0.0f, -sinOmega, cosOmega);
	XMVECTOR c0Vec = XMLoadFloat4(&c0);
	XMVECTOR c1Vec = XMLoadFloat4(&c1);

	XMVECTOR resultVec = XMQuaternionMultiply(htilde0Vec, c0Vec) + XMQuaternionMultiply(htilde0ConjVec, c1Vec);
	XMFLOAT2 result(resultVec.m128_f32[3], resultVec.m128_f32[2]);

	return result;
}

void OceanModel::GetHDN(float t, XMFLOAT2 X, XMFLOAT2 & height, XMFLOAT2 & displacement, XMFLOAT3 & normal)
{
	height.x = 0; height.y = 0;
	displacement.x = 0; displacement.y = 0;
	normal.x = 0; normal.y = 0; normal.z = 0;

	XMVECTOR dVec = XMLoadFloat2(&displacement), nVec = XMLoadFloat3(&normal);

	XMFLOAT4 c(0.0f, 0.0f, 0.0f, 0.0f), htildeMulC(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT2 k;
	float kx, kz, kLen, kDotX;

	for(int i=0; i<m_pointCount; i++)
	{
		kz = 2.0f * XM_PI * (i - m_gridCount / 2.0f) / m_width;
		for (int j = 0; j < m_pointCount; j++)
		{
			kx = 2.0f * XM_PI * (j - m_gridCount / 2.0f) / m_width;
			k.x = kx; k.y = kz;
			XMVECTOR kVec = XMLoadFloat2(&k);
			XMVECTOR kLenVec = XMVector2Length(kVec);
			kLen = kLenVec.m128_f32[0];

			XMVECTOR XVec = XMLoadFloat2(&X);
			XMVECTOR kDotXVec = XMVector2Dot(XVec, kVec);
			kDotX = kDotXVec.m128_f32[0];
			
			c.w = cos(kDotX); c.z = sin(kDotX);
			XMFLOAT2 htilde = GetHeightTilde(t, i, j);
			htildeMulC.w = htilde.x; htildeMulC.z = htilde.y;
			XMVECTOR htildeMulCVec, cVec;
			htildeMulCVec = XMLoadFloat4(&htildeMulC);
			cVec = XMLoadFloat4(&c);
			htildeMulCVec = XMQuaternionMultiply(htildeMulCVec, cVec);

			height.x += htildeMulCVec.m128_f32[3]; height.y += htildeMulCVec.m128_f32[2];
			nVec = nVec + XMVectorSet(-kx * htildeMulCVec.m128_f32[2], 0.0f, -kz * htildeMulCVec.m128_f32[2], 0.0f);
				
			if (kLen < 0.000001) continue;
			dVec = dVec + XMVectorSet(kx / kLen * htildeMulCVec.m128_f32[2], kz / kLen * htildeMulCVec.m128_f32[2], 0.0f, 0.0f);
		}
	}

	nVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) - nVec;
	nVec = XMVector3Normalize(nVec);

	XMStoreFloat2(&displacement, dVec);
	XMStoreFloat3(&normal, nVec);
}

