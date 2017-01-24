#include "Camera.h"
#include <iostream>
XMMATRIX camView;
XMMATRIX camProjection;
void Camera::init(int w,int h)
{
	fov = 0.25*3.14f;
	zNear = 0.01f;
	zFar = 1000.0f;
	position = XMFLOAT3(0.0f, 0.0f, -50.0);
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	updateRight();
	updateViewMatrix();
	width = w;
	height = h;

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

void Camera::updateViewMatrix()
{
	camView = XMMatrixIdentity();
	camView(0, 0) = right.x;
	camView(1, 0) = right.y;
	camView(2, 0) = right.z;

	camView(0, 1) = up.x;
	camView(1, 1) = up.y;
	camView(2, 1) = up.z;

	camView(0, 2) = look.x;
	camView(1, 2) = look.y;
	camView(2, 2) = look.z;

	XMVECTOR Q = XMLoadFloat3(&position);
	XMVECTOR u = XMLoadFloat3(&right);
	XMVECTOR v = XMLoadFloat3(&up);
	XMVECTOR w = XMLoadFloat3(&look);

	camView(3, 0) = -XMVectorGetX(XMVector3Dot(Q, u));
	camView(3, 1) = -XMVectorGetX(XMVector3Dot(Q, v));
	camView(3, 2) = -XMVectorGetX(XMVector3Dot(Q, w));
}

void Camera::updateRight()
{
	XMVECTOR look_dir = XMLoadFloat3(&look);
	XMVECTOR u = XMLoadFloat3(&up);
	XMVECTOR r = XMVector3Cross(XMVector3Normalize(u), XMVector3Normalize(look_dir));
	XMStoreFloat3(&right, r);
}

void Camera::walkForward(float d)
{
	position.x += d*look.x;
	position.y += d*look.y;
	position.z += d*look.z;
	updateViewMatrix();
}
void Camera::walkRight(float d)
{
	position.x += d*right.x;
	position.y += d*right.y;
	position.z += d*right.z;

	updateViewMatrix();
}

void Camera::walkUp(float d)
{
	position.y += d;
	updateViewMatrix();
}

void Camera::rotateY(float angle)//angle is radian
{
	XMMATRIX R = XMMatrixRotationAxis(XMVectorSet(0,1,0,0),angle);
	XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	updateViewMatrix();
}
void Camera::rotateRight(float angle)//angle is radian
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&right), angle);
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
	updateViewMatrix();
}