#ifndef VERTEX_H
#define VERTEX_H
#include "d3dUtil.h"
#include "Effects\Effects.h"
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float nx, float ny, float nz,float tx,float ty)
		: pos(x, y, z), normal(nx, ny, nz),tex(tx,ty){}

	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};

class InputLayouts
{
public:
	static void initAll(ID3D11Device* device, ID3DX11EffectTechnique* technique);
	static void destroyAll();
	static ID3D11InputLayout* PosNormal;
};

#endif