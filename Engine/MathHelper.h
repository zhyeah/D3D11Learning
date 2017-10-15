#ifndef _MATHHELPER_H_
#define _MATHHELPER_H_

#include <math.h>
#include <cstdlib>
#include <directxmath.h>
#include <random>
using namespace DirectX;

XMMATRIX InverseTranspose(CXMMATRIX M);

float GaussRand();

#endif // !_MATHHELPER_H_

