#ifndef CUBEMAPAPP_H
#include "AltarApp.h"
#include "Sky.h"
class CubeMapApp: public AltarApp
{
public:
	CubeMapApp();
	~CubeMapApp();
	void cleanUp();
	void initScene(int width, int height);
	void renderScene();
protected:
	Sky* m_sky;
};


#endif CUBEMAPAPP_H