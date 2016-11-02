#ifndef CRATEAPP_H
#define CRATEAPP_H
#include "D3DApp.h"
class CrateApp :public D3DApp
{
public:
	void cleanUp();
protected:
	void loadTextures();
	void renderScene();
	ID3D11ShaderResourceView* m_boxTexView;
};

#endif