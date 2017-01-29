#ifndef D3DAPP_H
#define D3DAPP_H
#include <QKeyEvent>
#include <d3dUtil.h>
#include <LightHelper.h>
#include <d3dx11effect.h>
#include <vector>
#include "Camera.h"
#include "Effects/Effects.h"
#include "Vertex.h"

class GameTimer;
//the class encapsulate the operations about D3D
class D3DApp
{
protected:
	struct MaterialData
	{
		std::string name;
		Material data;
	};

public:
	D3DApp();
	virtual ~D3DApp();
	virtual bool initD3D(HWND windowId, int width, int height);
	virtual void initScene(int width, int height);
	virtual void renderScene();
	virtual void cleanUp();
	virtual void updateScene(GameTimer*gameTimer);
	virtual void keyPressEvent(QKeyEvent *event);
	void resizeD3D(int width, int height);
	void setTransMat(float*data);
	void setTranslate(float x,float y);
	void loadObjData();
protected:
	virtual void initMaterials();
	void initLight();
	void loadShaders();
	void createViewport(int width,int height);
	void setRasterizationState();
	void buildVertexLayout();
	void saveAlphaImage(int width, int height, int alpha);
	ID3D11BlendState* initBlending();
	virtual void loadTextures();

	D3D11_VIEWPORT m_screenViewport;
	ID3D11Device *m_d3dDevice;
	ID3D11DeviceContext *m_d3dDevContext;
	IDXGISwapChain *m_swapChain;
	ID3D11Texture2D *m_depthStencilBuffer;
	ID3D11DepthStencilView *m_depthStencilView;
	ID3D11RenderTargetView *m_renderTargetView;
	ID3D11RasterizerState *m_rasterizeState;
	ID3D11BlendState* m_blendState;

	ID3D11Buffer *m_squareVertexBuffer;
	ID3D11Buffer *m_squareIndiceBuffer;

	XMFLOAT4X4 m_transformMat;
	Camera* m_camera;
	bool m_hasTex;

	//Light
	std::vector<DirectionalLight> m_dirLight;
	SpotLight m_spotLight;
	PointLight m_pointLight;

	std::vector<MaterialData> m_materials;
	float m_translateX, m_translateY;
};

#endif