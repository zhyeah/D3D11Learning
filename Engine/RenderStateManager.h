#ifndef _RENDERSTATEMANAGER_H_
#define _RENDERSTATEMANAGER_H_

#include<d3d11.h>

class RenderStateManager
{
public:
	static ID3D11BlendState* DisableBlendState(ID3D11Device*);
	static ID3D11BlendState* DisableWriteRenderTargetBlendState(ID3D11Device*);
	static ID3D11BlendState* SrcAlphaBlendSate(ID3D11Device*);

	static ID3D11DepthStencilState* NormalDepthStencilState(ID3D11Device*);
	static ID3D11DepthStencilState* MarkMirrorDepthStencilState(ID3D11Device*);
	static ID3D11DepthStencilState* DrawReflectDepthStencilState(ID3D11Device*);
};

#endif // !_RENDERSTATEMANAGER_H_

