#ifndef _TEXTUREBASE_H_
#define _TEXTUREBASE_H_

#include <d3d11.h>
#include "WICTextureLoader.h"
using namespace DirectX;

class TextureBase
{
public:
	TextureBase();
	TextureBase(const TextureBase&);
	~TextureBase();

	bool Initialize(ID3D11Device*, LPCWSTR);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void SetTexture(ID3D11ShaderResourceView*);

protected:
	ID3D11ShaderResourceView* m_textureView;
};

#endif // !_TEXTUREBASE_H_