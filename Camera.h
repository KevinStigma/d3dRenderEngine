#ifndef ZUK_CAMERA_H
#define ZUK_CAMERA_H
#include <d3d11.h>
#include <xnamath.h>
class Camera
{
public:
	void init();
	XMFLOAT4 position;
	XMFLOAT4 target;
	XMFLOAT4 up;
	float fov;
	float zNear;
	float zFar;
};

#endif