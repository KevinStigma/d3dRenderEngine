#ifndef PARTICLEAPP_H
#define PARTICLEAPP_H
#include "D3DApp.h"
#include "Sky.h"
#include "Terrain.h"
#include "ParticleSystem.h"
class ParticleApp:public D3DApp
{
public:
	ParticleApp();
	void initScene(int width, int height);
	void renderScene();
	void cleanUp();
	void updateScene(GameTimer*gameTimer);
	void keyPressEvent(QKeyEvent *event);
protected:
	Sky* m_sky;
	Terrain m_terrain;
	ID3D11ShaderResourceView* m_flareTexSRV;
	ID3D11ShaderResourceView* m_randomTexSRV;
	ParticleSystem m_fire;
};

#endif