#ifndef _FFT_H_
#define _FFT_H_

#include <d3d11.h>
#include <directxmath.h>
#include <math.h>
using namespace DirectX;

class FFT
{
public:
	FFT();
	~FFT();
	void Initialize(UINT N);
	void Release();
	UINT Reverse(UINT i);
	XMFLOAT2 GetT(UINT x, UINT N);
	void FastFourierTransform(XMFLOAT2* input, XMFLOAT2* output, int stride, int offset);

private:
	UINT m_N, m_which;
	UINT m_log2N;
	UINT* m_reversed;
	XMFLOAT2** m_T;
	XMFLOAT2* m_c[2];
};

#endif