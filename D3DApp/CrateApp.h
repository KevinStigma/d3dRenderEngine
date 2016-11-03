#ifndef CRATEAPP_H
#define CRATEAPP_H
#include "D3DApp.h"
class CrateApp :public D3DApp
{
public:
	void cleanUp();
	void renderScene();
protected:
	void loadTextures();
	ID3D11ShaderResourceView* m_boxTexView;
};

#endif