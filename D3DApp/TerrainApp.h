#ifndef TERRAINAPP_H
#define TERRAINAPP_H
#include "D3DApp.h"
#include "Sky.h"
#include "Terrain.h"

class TerrainApp:public D3DApp
{
public:
	TerrainApp();
	virtual bool initD3D(HWND windowId, int width, int height);
	virtual void initScene(int width, int height);
	virtual void renderScene();
	virtual void cleanUp();
	virtual void updateScene(GameTimer*gameTimer);
	void keyPressEvent(QKeyEvent *e);
protected:
	Sky* m_sky;
	Terrain m_terrain;
};

#endif