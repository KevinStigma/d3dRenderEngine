#ifndef NORMALDISPLACEMENTMAPAPP_H
#define NORMALDISPLACEMENTMAPAPP_H
#include "AltarApp.h"
#include "Sky.h"

enum RenderOption
{
	RENDER_BASIC,
	RENDER_NORMALMAP,
	RENDER_DISPLACEMENTMAP
};
class NormalDisplacementMapApp:public AltarApp
{	
public:
	NormalDisplacementMapApp();
	virtual void initScene(int width, int height);
	virtual void updateScene(GameTimer*gameTimer);
	virtual void renderScene();
	virtual void cleanUp();
	virtual void keyPressEvent(QKeyEvent* event);
protected:
	void loadTextures();
	ID3D11ShaderResourceView* m_stoneNormalTexSRV;
	ID3D11ShaderResourceView* m_brickNormalTexSRV;
	
	RenderOption m_renderOption;
	Sky* m_sky;
};
#endif