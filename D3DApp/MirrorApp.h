#ifndef MIRRORAPP_H
#define MIRRORAPP_H
#include "D3DApp.h"

class MirrorApp:public D3DApp
{
public:
	MirrorApp();
	void initScene(int width, int height);
	void updateScene(GameTimer*gameTimer);
	void renderScene();
	void cleanUp();
protected:
	//void initMaterials();
	void loadTextures();
	void buildRoomGeometryBuffers();
	void buildSkullGeometryBuffers();

	int m_skullIndexCount;
	XMFLOAT3 m_skullTranslation;
	XMFLOAT4X4 m_skullWorld;

	ID3D11Buffer* m_roomVB;
	ID3D11Buffer* m_skullVB;
	ID3D11Buffer* m_skullIB;
	ID3D11ShaderResourceView* m_floorDiffuseMapSRV;
	ID3D11ShaderResourceView* m_wallDiffuseMapSRV;
	ID3D11ShaderResourceView* m_mirrorDiffuseMapSRV;

};


#endif