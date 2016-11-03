#ifndef HILLWAVEAPP_H
#define HILLWAVEAPP_H
#include "D3DApp.h"
#include <Waves.h>
class HillWaveApp:public D3DApp
{
public:
	HillWaveApp();
	void cleanUp();
	void initScene(int width, int height);
	void renderScene();
	void updateScene(GameTimer*gameTimer);
protected:
	void loadTextures();
	void buildLandGeometryBuffers();
	void buildWaveGeometryBuffers();
	ID3D11ShaderResourceView* m_grassMapSRV;
	ID3D11ShaderResourceView* m_wavesMapSRV;

	int m_landIndexCount;
	ID3D11Buffer* m_landVB;
	ID3D11Buffer* m_landIB;
	ID3D11Buffer* m_wavesIB;
	ID3D11Buffer* m_wavesVB;
	Waves m_waves;
	XMFLOAT2 m_waterTexOffset;
	XMFLOAT4X4 m_waterTexTransform;
	XMFLOAT4X4 m_grassTexTransform;
};

#endif