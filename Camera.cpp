#include "Camera.h"
#include <iostream>

void Camera::init()
{
	fov = 0.25f* XM_PI;
	zNear = 0.01f;
	zFar = 1000.0f;
	position = XMFLOAT3(0.0f, 0.0f, -50.0);
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	updateRight();

	aspectRatio = 800.f/600;
	setLens(fov, aspectRatio, zNear, zFar);
	
	updateViewMatrix();
}

XMMATRIX Camera::getViewMatrix()const 
{
	return XMLoadFloat4x4(&mView);
}
XMMATRIX Camera::getProjMatrix()const
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::getViewProjMatrix()const
{
	return XMLoadFloat4x4(&mViewProj);
}

void Camera::setLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	fov = fovY;
	aspectRatio = aspect;
	zNear = zn;
	zFar = zf;

	XMStoreFloat4x4(&mProj,XMMatrixPerspectiveFovLH(fov, aspectRatio, zNear, zFar));
}

void Camera::lookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&position, pos);
	XMStoreFloat3(&look, L);
	XMStoreFloat3(&right, R);
	XMStoreFloat3(&up, U);
}

void Camera::updateViewMatrix()
{
	XMMATRIX camView = XMMatrixIdentity();
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

	XMMATRIX camViewProj = camView*XMLoadFloat4x4(&mProj);
	XMStoreFloat4x4(&mView, camView);
	XMStoreFloat4x4(&mViewProj, camViewProj);
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