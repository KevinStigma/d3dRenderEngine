#ifndef D3DAPP_H
#define D3DAPP_H
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <LightHelper.h>
#include <d3dx11effect.h>
#include <vector>
#include "Camera.h"

//the class encapsulate the operations about D3D
class D3DApp
{
protected:
	struct Vertex
	{
		Vertex(){}
		Vertex(float x, float y, float z,
			float nx, float ny, float nz)
			: pos(x, y, z), normal(nx,ny,nz){}

		XMFLOAT3 pos;
		XMFLOAT3 normal;
	};

	struct MaterialData
	{
		std::string name;
		Material data;
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
	void setTransMat(float*data);
	void setTranslate(float x,float y);
	void loadObjData();
protected:
	void initMaterials();
	void initLight();
	void loadShaders();
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

	ID3DX11EffectMatrixVariable* m_fxWorldViewProj;
	ID3DX11EffectMatrixVariable* m_fxWorld;
	ID3DX11EffectMatrixVariable* m_fxWorldInvTranspose;
	ID3DX11EffectVectorVariable* m_fxEyePosW;
	ID3DX11EffectTechnique* m_fxTech;
	ID3DX11EffectVariable* m_fxDirLight;
	ID3DX11EffectVariable* m_fxPointLight;
	ID3DX11EffectVariable* m_fxSpotLight;
	ID3DX11EffectVariable* m_fxMaterial;

	XMFLOAT4X4 m_transformMat;
	Camera m_camera;

	//Light
	std::vector<DirectionalLight> m_dirLight;
	SpotLight m_spotLight;
	PointLight m_pointLight;

	std::vector<MaterialData> m_materials;
	float m_translateX, m_translateY;
};

#endif