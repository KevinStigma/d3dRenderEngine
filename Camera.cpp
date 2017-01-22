#include "Camera.h"
XMMATRIX camView;
XMMATRIX camProjection;
void Camera::init(int w,int h)
{
	fov = 0.25*3.14f;
	zNear = 0.01f;
	zFar = 1000.0f;
	position = XMFLOAT4(0.0f, 0.0f, -50.0f,1.0f);
	target = XMFLOAT4(0.0f, 0.0f, 0.0f,0.0f);
	up = XMFLOAT4(0.0f, 1.0f, 0.0f,0.0f);
	width = w;
	height = h;
	camView = XMMatrixLookAtLH(XMLoadFloat4(&position), XMLoadFloat4(&target), XMLoadFloat4(&up));
	camProjection = XMMatrixPerspectiveFovLH(fov, (float)width / height, zNear, zFar);
}

XMMATRIX Camera::getViewMatrix()
{
	return camView;
}
XMMATRIX Camera::getProjMatrix()
{
	return camProjection;
}

void Camera::calViewMatrix(XMFLOAT3 pos,XMFLOAT3 t,XMFLOAT3 u)
{
	position = XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
	target = XMFLOAT4(t.x, t.y, t.z, 0.0f);
	up = XMFLOAT4(u.x, u.y, u.z, 0.0f);
	camView = XMMatrixLookAtLH(XMLoadFloat4(&position), XMLoadFloat4(&target), XMLoadFloat4(&up));
}