#include "NormalShader.h"
using namespace DirectX;

NormalShader::NormalShader() 
{
	m_cbPerFrame = NULL;
	m_sampleState = NULL;
}

NormalShader::NormalShader(const NormalShader& normalShader) : ShaderBase(normalShader)
{}

NormalShader::~NormalShader()
{}

void NormalShader::SetDirectionalLight(DirectionalLight& directionLight)
{
	m_directionalLight = directionLight;
}

void NormalShader::SetPointLight(PointLight& pointLight)
{
	m_pointLight = pointLight;
}

void NormalShader::SetSpotLight(SpotLight& spotLight)
{
	m_spotLight = spotLight;
}

void NormalShader::SetEyePosition(XMFLOAT3& eyePosition)
{
	m_eyePosition = eyePosition;
}

void NormalShader::SetMaterial(Material& material)
{
	m_material = material;
}

void NormalShader::ShutdownShader()
{
	if (m_cbPerFrame) 
	{
		m_cbPerFrame->Release();
		m_cbPerFrame = NULL;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = NULL;
	}

	ShaderBase::ShutdownShader();
}

void NormalShader::SetVertextInputDesc(D3D11_INPUT_ELEMENT_DESC** inputElementDesc, unsigned int& numElements)
{
	numElements = 3;
	*inputElementDesc = new D3D11_INPUT_ELEMENT_DESC[numElements];

	(*inputElementDesc)[0].SemanticName = "POSITION";
	(*inputElementDesc)[0].SemanticIndex = 0;
	(*inputElementDesc)[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	(*inputElementDesc)[0].InputSlot = 0;
	(*inputElementDesc)[0].AlignedByteOffset = 0;
	(*inputElementDesc)[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	(*inputElementDesc)[0].InstanceDataStepRate = 0;

	(*inputElementDesc)[1].SemanticName = "NORMAL";
	(*inputElementDesc)[1].SemanticIndex = 0;
	(*inputElementDesc)[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	(*inputElementDesc)[1].InputSlot = 0;
	(*inputElementDesc)[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	(*inputElementDesc)[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	(*inputElementDesc)[1].InstanceDataStepRate = 0;

	(*inputElementDesc)[2].SemanticName = "TEXCOORD";
	(*inputElementDesc)[2].SemanticIndex = 0;
	(*inputElementDesc)[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	(*inputElementDesc)[2].InputSlot = 0;
	(*inputElementDesc)[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	(*inputElementDesc)[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	(*inputElementDesc)[2].InstanceDataStepRate = 0;
}

bool NormalShader::InitializeConstantBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC cbPerFrameBufferDesc;
	HRESULT result;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	cbPerFrameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbPerFrameBufferDesc.ByteWidth = sizeof(CBPerFrame);
	cbPerFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbPerFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbPerFrameBufferDesc.MiscFlags = 0;
	cbPerFrameBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cbPerFrameBufferDesc, NULL, &(this->m_cbPerFrame));
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC cbPerObjectBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	cbPerObjectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbPerObjectBufferDesc.ByteWidth = sizeof(CBPerObject);
	cbPerObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbPerObjectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbPerObjectBufferDesc.MiscFlags = 0;
	cbPerObjectBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cbPerObjectBufferDesc, NULL, &(this->m_cbPerObject));
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool NormalShader::InitializeSampleState(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	HRESULT result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool NormalShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, ModelBase* model)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedFrameResource;
	CBPerFrame* cbFramePtr;
	unsigned int bufferNumber;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_cbPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedFrameResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	cbFramePtr = (CBPerFrame*)mappedFrameResource.pData;

	// Copy the matrices into the constant buffer.
	cbFramePtr->directionLight = m_directionalLight;
	cbFramePtr->pointLight = m_pointLight;
	cbFramePtr->spotLight = m_spotLight;
	cbFramePtr->eyePosition = XMLoadFloat3(&m_eyePosition);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_cbPerFrame, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cbPerFrame);
	deviceContext->PSSetConstantBuffers(bufferNumber++, 1, &m_cbPerFrame);

	D3D11_MAPPED_SUBRESOURCE mappedObjectResource;
	CBPerObject* cbObjectPtr;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_cbPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	cbObjectPtr = (CBPerObject*)mappedObjectResource.pData;

	// Copy the matrices into the constant buffer.
	cbObjectPtr->world = XMMatrixTranspose(m_world);
	cbObjectPtr->inverseTransposeWorld = XMMatrixTranspose(InverseTranspose(m_world));
	cbObjectPtr->worldViewProj = XMMatrixTranspose(m_world * m_view * m_project); //DirectX::XMMatrixTranspose(m_project * m_view * m_world);
	cbObjectPtr->material = model->GetMaterial();
	cbObjectPtr->useTexture = model->GetUseTexture();

	// Unlock the constant buffer.
	deviceContext->Unmap(m_cbPerObject, 0);

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cbPerObject);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_cbPerObject);

	return true;
}