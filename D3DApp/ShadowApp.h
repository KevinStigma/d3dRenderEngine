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
	void initScene(int width, int height);
	void updateScene(GameTimer*gameTimer);
	void renderScene();
	void cleanUp();
protected:
	void buildShadowTransform();
	void drawSceneToShadowMap();
	float m_lightRotationAngle;
	BoundingSphere m_sceneBounds;
	XMFLOAT3 m_originalLightDir[3];
	XMFLOAT4X4 m_lightView;
	XMFLOAT4X4 m_lightProj;
	XMFLOAT4X4 m_shadowTransform;
	Sky* m_sky;
	ShadowMap* m_smap;
	int m_shadowMapSize;
};
#endif