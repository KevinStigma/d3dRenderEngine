#ifndef ANIMATIONAPP_H
#define ANIMATIONAPP_H
#include "ShadowApp.h"
#include "TextureMgr.h"
#include "SkinnedModel.h"
class AnimationApp:public ShadowApp
{
public:
	AnimationApp();
	virtual void initScene(int width, int height);
	virtual void updateScene(GameTimer*gameTimer);
	virtual void renderScene();
	virtual void cleanUp();
protected:
	void drawSceneToShadowMap();
	SkinnedModel* m_characterModel;
	SkinnedModelInstance m_characterInstance1;
	SkinnedModelInstance m_characterInstance2;
	ID3D11ShaderResourceView* m_headTexSRV;
	TextureMgr m_texMgr;
};

#endif 