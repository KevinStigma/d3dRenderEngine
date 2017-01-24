#ifndef ZYK_CAMERA_H
#define ZYK_CAMERA_H
#include <d3d11.h>
#include <xnamath.h>
class Camera
{
public:
	void init(int w,int h);
	XMMATRIX getViewMatrix();
	XMMATRIX getProjMatrix();
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