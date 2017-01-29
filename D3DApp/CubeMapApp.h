#ifndef CUBEMAPAPP_H
#include "AltarApp.h"
#include "Sky.h"
class CubeMapApp: public AltarApp
{
public:
	CubeMapApp();
	~CubeMapApp();
	void cleanUp();
	bool initD3D(HWND windowId, int width, int height);
	void initScene(int width, int height);
	void renderScene();
protected:
	void buildDynamicCubeMapViews();
	void buildCubeFaceCamera(float x, float y, float z);
	void drawScene(const Camera& camera, bool drawCenterSphere);
	Sky* m_sky;
	XMFLOAT4X4 m_centerSphereWorld;
	ID3D11RenderTargetView* m_dynamicCubeMapRTV[6];
	ID3D11ShaderResourceView* m_dynamicCubeMapSRV;
 	ID3D11DepthStencilView* m_dynamicCubeMapDSV;
	D3D11_VIEWPORT m_cubeMapViewport;
	static const int CubeMapSize = 256;
	Camera m_cubeMapCamera[6];
};


#endif CUBEMAPAPP_H