#ifndef RENDERSTATES_H
#define RENDERSTATES_H
#include <d3dUtil.h>
class RenderStates
{
public:
	static void initAll(ID3D11Device* device);
	static void destroyAll();
	static ID3D11DepthStencilState* EqualDSS;
	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* CullClockwiseRS;
	static ID3D11BlendState* TransparentBS;
};
#endif