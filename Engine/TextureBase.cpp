#include "TextureBase.h"

TextureBase::TextureBase()
{
	m_textureView = NULL;
}

TextureBase::TextureBase(const TextureBase&)
{
}

TextureBase::~TextureBase()
{
}

bool TextureBase::Initialize(ID3D11Device* device, LPCWSTR filename)
{
	HRESULT result = CreateWICTextureFromFile(device, filename, NULL, &m_textureView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureBase::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = NULL;
	}
}

ID3D11ShaderResourceView* TextureBase::GetTexture()
{
	return m_textureView;
}

void TextureBase::SetTexture(ID3D11ShaderResourceView* shaderResourceView)
{
	// If not null, release it.
	if (m_textureView != NULL)
	{
		m_textureView->Release();
	}

	m_textureView = shaderResourceView;
}
