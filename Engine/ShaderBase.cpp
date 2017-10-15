#include "ShaderBase.h"
#include "ModelBase.h"

ShaderBase::ShaderBase() 
{
	this->m_indexCount = 0;
	this->m_indexOffset = 0;

	this->m_layout = NULL;
	this->m_cbPerObject = NULL;
	this->m_vertexShader = NULL;
	this->m_pixelShader = NULL;
}

ShaderBase::ShaderBase(const ShaderBase&) 
{
}

ShaderBase::~ShaderBase() 
{
}

bool ShaderBase::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, LPCSTR vsEntry, WCHAR* psFilename, LPCSTR psEntry)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = this->InitializeShader(device, hwnd, vsFilename, vsEntry, psFilename, psEntry);
	if (!result)
	{
		return false;
	}

	return true;
}

void ShaderBase::Shutdown() 
{
	this->ShutdownShader();
}

bool ShaderBase::Render(ID3D11DeviceContext* deviceContext, ModelBase* model)
{
	bool result = this->SetShaderParameters(deviceContext, model);
	if (!result)
	{
		return false;
	}

	this->RenderShader(deviceContext, model);

	return true;
}

void ShaderBase::SetIndexCount(int indexCount) 
{
	this->m_indexCount = indexCount;
}

void ShaderBase::SetIndexOffset(int indexOffset)
{
	this->m_indexOffset = indexOffset;
}

void ShaderBase::SetWorldMatrix(const XMMATRIX& world)
{
	m_world = world;
}

void ShaderBase::SetViewMatrix(const XMMATRIX& view)
{
	m_view = view;
}

void ShaderBase::SetProjectMatrix(const XMMATRIX& project)
{
	m_project = project;
}

bool ShaderBase::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, LPCSTR vsEntry, WCHAR* psFilename, LPCSTR psEntry) 
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	// Initialize the pointers this function will use to null.
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, vsEntry, "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			this->OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, psEntry, "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			this->OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &(this->m_vertexShader));
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &(this->m_pixelShader));
	if (FAILED(result))
	{
		return false;
	}

	// Initial the pointer outside, so when dev derive the class, he didn't need to worry about its initialization.
	D3D11_INPUT_ELEMENT_DESC** inputElementsDesc = new D3D11_INPUT_ELEMENT_DESC*;
	unsigned int numElements = 0;

	this->SetVertextInputDesc(inputElementsDesc, numElements);

	// Create the vertex input layout.
	result = device->CreateInputLayout(*inputElementsDesc, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &(this->m_layout));
	if (FAILED(result))
	{
		return false;
	}

	// Release inputElementsDesc.
	delete[](*inputElementsDesc);
	*inputElementsDesc = NULL;
	delete inputElementsDesc;
	inputElementsDesc = NULL;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	result = this->InitializeSampleState(device);
	if (FAILED(result))
	{
		return false;
	}

	return this->InitializeConstantBuffer(device);
}

void ShaderBase::SetVertextInputDesc(D3D11_INPUT_ELEMENT_DESC** inputElementDesc, unsigned int& numElements)
{
	// Left for derived class.
}

bool ShaderBase::InitializeConstantBuffer(ID3D11Device* device)
{
	// Left for derived class.
	return true;
}

bool ShaderBase::InitializeSampleState(ID3D11Device* device)
{
	// Left for derived class.
	return true;
}

void ShaderBase::ShutdownShader() 
{
	// Release the matrix constant buffer.
	if (this->m_cbPerObject)
	{
		this->m_cbPerObject->Release();
		this->m_cbPerObject = 0;
	}

	// Release the layout.
	if (this->m_layout)
	{
		this->m_layout->Release();
		this->m_layout = 0;
	}

	// Release the pixel shader.
	if (this->m_pixelShader)
	{
		this->m_pixelShader->Release();
		this->m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = 0;
	}
}

void ShaderBase::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ShaderBase::SetShaderParameters(ID3D11DeviceContext* deviceContext, ModelBase* model)
{
	// Left for child class.
	return true;
}

void ShaderBase::RenderShader(ID3D11DeviceContext* deviceContext, ModelBase* model)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(this->m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(this->m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(this->m_pixelShader, NULL, 0);

	if (model->GetUseTexture())
	{
		// Set the sampler state in the pixel shader.
		deviceContext->PSSetSamplers(0, 1, &m_sampleState);
		// Set shader texture resource in the pixel shader.
		ID3D11ShaderResourceView* shaderResourceView = model->GetTexture();
		deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
	}

	// Render the triangle.
	deviceContext->DrawIndexed(this->m_indexCount, this->m_indexOffset, 0);
}