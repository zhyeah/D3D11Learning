#include "FFT.h"

FFT::FFT()
{
	m_N = 0;
	m_which = 0;
	m_log2N = 0;
	m_reversed = NULL;
	m_T = NULL;
	m_c[0] = NULL;
	m_c[1] = NULL;
}

FFT::~FFT()
{
}

void FFT::Initialize(UINT N)
{
	m_N = N;
	m_log2N = log(N) / log(2);
	m_which = 0;

	m_reversed = new UINT[N];
	for (UINT i = 0; i < N; i++)
	{
		m_reversed[i] = Reverse(i);
	}

	int pow = 1;
	m_T = new XMFLOAT2*[m_log2N];
	for (UINT i = 0; i < m_log2N; i++, pow*=2)
	{
		m_T[i] = new XMFLOAT2[pow];
		for (UINT j = 0; j < pow; j++)
		{
			m_T[i][j] = GetT(j, pow * 2);
		}
	}

	m_c[0] = new XMFLOAT2[N];
	m_c[1] = new XMFLOAT2[N];
}

void FFT::Release()
{
	if (m_c[0])
	{
		delete[] m_c[0];
		m_c[0] = NULL;
	}

	if (m_c[1])
	{
		delete[] m_c[1];
		m_c[1] = NULL;
	}

	if (m_reversed)
	{
		delete[] m_reversed;
		m_reversed = NULL;
	}

	if (m_T)
	{
		for (UINT i = 0; i < m_log2N; i++)
		{
			delete[] m_T[i];
			m_T[i] = NULL;
		}

		delete[] m_T;
		m_T = NULL;
	}
}

UINT FFT::Reverse(UINT i)
{
	UINT result = 0;
	for (UINT j = 0; j < m_log2N; j++)
	{
		result = (result << 1) + (i & 1);
		i >>= 1;
	}
	
	return result;
}

XMFLOAT2 FFT::GetT(UINT x, UINT N)
{
	return XMFLOAT2(cos(XM_2PI * x / N), sin(XM_2PI * x / N));
}

void FFT::FastFourierTransform(XMFLOAT2 * input, XMFLOAT2 * output, int stride, int offset)
{
	for (UINT i = 0; i < m_N; i++)
	{ 
		m_c[m_which][i] = input[m_reversed[i] * stride + offset];
	}

	UINT loops = m_N >> 1;
	UINT currentSize = 2;
	UINT halfSize = currentSize / 2;
	for (UINT i = 1; i <= m_log2N; i++)
	{
		m_which ^= 1;
		for (UINT j = 0; j < loops; j++)
		{
			// upper half.
			for (UINT k = 0; k < halfSize; k++)
			{
				XMVECTOR a = XMVectorSet(0.0f, 0.0f, m_c[m_which ^ 1][currentSize * j + k].y, m_c[m_which ^ 1][currentSize * j + k].x);
				XMVECTOR b = XMVectorSet(0.0f, 0.0f, m_c[m_which ^ 1][currentSize * j + k + halfSize].y, m_c[m_which ^ 1][currentSize * j + k + halfSize].x);
				XMVECTOR c = XMVectorSet(0.0f, 0.0f, m_T[i-1][k].y, m_T[i-1][k].x);
				XMVECTOR prod = XMQuaternionMultiply(b, c);

				m_c[m_which][currentSize * j + k].x = a.m128_f32[3] + prod.m128_f32[3];
				m_c[m_which][currentSize * j + k].y = a.m128_f32[2] + prod.m128_f32[2];
			}

			// lower half.
			for (UINT k = halfSize; k < currentSize; k++)
			{
				XMVECTOR a = XMVectorSet(0.0f, 0.0f, m_c[m_which ^ 1][currentSize * j + k - halfSize].y, m_c[m_which ^ 1][currentSize * j + k - halfSize].x);
				XMVECTOR b = XMVectorSet(0.0f, 0.0f, m_c[m_which ^ 1][currentSize * j + k].y, m_c[m_which ^ 1][currentSize * j + k].x);
				XMVECTOR c = XMVectorSet(0.0f, 0.0f, m_T[i-1][k - halfSize].y, m_T[i-1][k - halfSize].x);
				XMVECTOR prod = XMQuaternionMultiply(b, c);

				m_c[m_which][currentSize * j + k].x = a.m128_f32[3] - prod.m128_f32[3];
				m_c[m_which][currentSize * j + k].y = a.m128_f32[2] - prod.m128_f32[2];
			}
		}

		loops >>= 1;
		currentSize <<= 1;
		halfSize <<= 1;
	}

	for (UINT i = 0; i < m_N; i++)
	{
		output[i * stride + offset] = m_c[m_which][i];
	}
}
