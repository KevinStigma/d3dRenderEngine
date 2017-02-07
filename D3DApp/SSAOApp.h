#ifndef SSAOAPP_H
#define SSAOAPP_H
#include "ShadowApp.h"
#include "../Ssao.h"
class SSAOApp :public ShadowApp
{
public:
	SSAOApp();
	virtual void initScene(int width, int height);
	virtual void updateScene(GameTimer*gameTimer);
	virtual void renderScene();
	virtual void cleanUp();
protected:
	void drawSceneToSsaoNormalDepthMap();
	void drawScreenQuad(ID3D11ShaderResourceView* srv);
	Ssao* m_ssao;
};

#endif