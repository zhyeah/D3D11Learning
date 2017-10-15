#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <directxmath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetLookAt(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetLookAt();

	void Render();
	void GetViewMatrix(XMMATRIX&);

protected:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_lookAtX, m_lookAtY, m_lookAtZ;
	XMMATRIX m_viewMatrix;
};

#endif // !_CAMERA_H_

