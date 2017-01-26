#ifndef ZYK_CAMERA_H
#define ZYK_CAMERA_H
#include <d3d11.h>
#include <xnamath.h>
//#include "MathHelper.h" this problem will be left to solve
class Camera
{
public:
	void init(int w,int h);
	XMMATRIX getViewMatrix()const;
	XMMATRIX getProjMatrix()const;
	XMMATRIX getViewProjMatrix()const;
	void walkForward(float d);
	void walkRight(float d);
	void walkUp(float d);
	void updateViewMatrix();
	void updateRight();
	void rotateY(float angle);
	void rotateRight(float angle);

	XMFLOAT3 position;
	XMFLOAT3 up;
	XMFLOAT3 right;
	XMFLOAT3 look;
	float fov;
	float zNear;
	float zFar;
	int width, height;
};

#endif