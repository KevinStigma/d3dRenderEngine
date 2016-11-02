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
	XMFLOAT4 position;
	XMFLOAT4 target;
	XMFLOAT4 up;
	float fov;
	float zNear;
	float zFar;
	int width, height;
};

#endif