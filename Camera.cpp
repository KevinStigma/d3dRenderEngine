#include "Camera.h"
void Camera::init()
{
	fov = 0.25*3.14f;
	zNear = 0.01f;
	zFar = 1000.0f;
	position = XMFLOAT4(0.0f, 3.0f, -8.0f,0.0f);
	target = XMFLOAT4(0.0f, 0.0f, 0.0f,0.0f);
	up = XMFLOAT4(0.0f, 1.0f, 0.0f,0.0f);
}