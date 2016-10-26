#ifndef D3DAPP_H
#define D3DAPP_H
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <d3dx11effect.h>
#include "Camera.h"

//the class encapsulate the operations about D3D
class D3DApp
{
private:
	struct Vertex
	{
		Vertex(){}
		Vertex(float x, float y, float z,
			float cr, float cg, float cb, float ca)
			: pos(x, y, z), color(cr, cg, cb, ca){}

		XMFLOAT3 pos;
		XMFLOAT4 color;
	};
	struct VS_ConstantBuffer
	{
		XMMATRIX WVP;
	};

public:
	D3DApp();
	~D3DApp();
	bool initD3D(HWND windowId, int width, int height);
	void initScene(int width, int height);
	void renderScene();
	void cleanUp();
	void updateScene(double deltaTime);
	void resizeD3D(int width, int height);
protected:
	void loadShaders();
	void loadData();
	void createViewport(int width,int height);
	void setRasterizationState();
	void buildVertexLayout();

	ID3D11Device *m_d3dDevice;
	ID3D11DeviceContext *m_d3dDevContext;
	IDXGISwapChain *m_swapChain;
	ID3D11Texture2D *m_depthStencilBuffer;
	ID3D11DepthStencilView *m_depthStencilView;
	ID3D11RenderTargetView *m_renderTargetView;
	ID3D11RasterizerState *m_rasterizeState;

	ID3D11Buffer *m_squareVertexBuffer;
	ID3D11Buffer *m_squareIndiceBuffer;

	ID3D11InputLayout *m_inputLayout;
	ID3DX11Effect*m_fx;
	ID3DX11EffectMatrixVariable* m_fxWVPVar;
	ID3DX11EffectTechnique* m_tech;

	Camera m_camera;
};

#endif