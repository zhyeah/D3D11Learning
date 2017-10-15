#include "MathHelper.h"
using namespace std;

XMMATRIX InverseTranspose(CXMMATRIX M)
{
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

float GaussRand()
{
	random_device rd;
	mt19937 gen(rd());

	normal_distribution<float> dis(0, 1);

	return dis(gen);
}
