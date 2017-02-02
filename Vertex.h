#ifndef VERTEX_H
#define VERTEX_H
#include "d3dUtil.h"
#include "Effects\Effects.h"

namespace Vertex
{
	// Basic 32-byte vertex structure.
	struct Basic32
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
		Basic32(float px=0,float py=0,float pz=0,float nx=0,float ny=0,float nz=0,float tu=0,float tv=0)
		{
			pos = XMFLOAT3(px, py, pz);
			normal = XMFLOAT3(nx, ny, nz);
			tex = XMFLOAT2(tu, tv);
		}
	};

	struct Terrain
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT2 boundsY;
	};
	struct PosNormalTexTan
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
		XMFLOAT3 tangentU;
		PosNormalTexTan(float px=0, float py=0, float pz=0, float nx=0, float ny=0, float nz=0, float tu=0, float tv=0, float tux=0, float tuy=0, float tuz=0)
		{
			pos = XMFLOAT3(px, py, pz);
			normal = XMFLOAT3(nx, ny, nz);
			tex = XMFLOAT2(tu, tv);
			tangentU = XMFLOAT3(tux,tuy, tuz);
		}
	};
}

class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC Terrain[3];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];
};


class InputLayouts
{
public:
	static void initAll(ID3D11Device* device);
	static void destroyAll();
	static ID3D11InputLayout* Pos;
	static ID3D11InputLayout* PosNorTex;
	static ID3D11InputLayout* PosNormalTexTan;
	static ID3D11InputLayout* Terrain;
};

#endif