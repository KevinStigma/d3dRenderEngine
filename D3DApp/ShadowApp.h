#ifndef SHADOWAPP_H
#define SHADOWAPP_H
#include "AltarApp.h"
#include "../ShadowMap.h"
#include "../Sky.h"

struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};
class ShadowApp:public AltarApp
{
public:
	ShadowApp();
	virtual void initScene(int width, int height);
	virtual void updateScene(GameTimer*gameTimer);
	virtual void renderScene();
	virtual void cleanUp();
protected:
	void buildShadowTransform();
	void drawSceneToShadowMap();
	void buildScreenQuadGeometryBuffers();
	float m_lightRotationAngle;
	BoundingSphere m_sceneBounds;
	XMFLOAT3 m_originalLightDir[3];
	XMFLOAT4X4 m_lightView;
	XMFLOAT4X4 m_lightProj;
	XMFLOAT4X4 m_shadowTransform;
	Sky* m_sky;
	ShadowMap* m_smap;
	int m_shadowMapSize;
	ID3D11Buffer* m_screenQuadVB;
	ID3D11Buffer* m_screenQuadIB;

};
#endif