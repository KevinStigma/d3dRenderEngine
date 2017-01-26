#ifndef ALTARAPP_H
#define ALTARAPP_H
#include <QKeyEvent>
#include "D3DApp.h"

class AltarApp:public D3DApp
{
public:
	AltarApp();
	virtual void initScene(int width, int height);
	virtual void updateScene(GameTimer*gameTimer);
	virtual void renderScene();
	virtual void cleanUp();
	void keyPressEvent(QKeyEvent* event);
protected:
	void buildShapeGeometryBuffers();
	void buildSkullGeometryBuffers();
	void initWorldMatrix();
	void initMaterials();
	void loadTextures();

	int m_gridVertexOffset;
	int m_boxVertexOffset;
	int m_sphereVertexOffset;
	int m_cylinderVertexOffset;

	int m_boxIndexCount;
	int m_gridIndexCount;
	int m_sphereIndexCount;
	int m_cylinderIndexCount;

	int m_boxIndexOffset;
	int m_gridIndexOffset;
	int m_sphereIndexOffset;
	int m_cylinderIndexOffset;
	int m_skullIndexCount;
	int m_lightCount;

	XMFLOAT4X4 m_gridWorld;
	XMFLOAT4X4 m_boxWorld;
	XMFLOAT4X4 m_skullWorld;
	XMFLOAT4X4 m_cylWorld[10];
	XMFLOAT4X4 m_sphereWorld[10];

	ID3D11ShaderResourceView *m_floorTexSRV;
	ID3D11ShaderResourceView *m_stoneTexSRV;
	ID3D11ShaderResourceView *m_brickTexSRV;
	
	ID3D11Buffer *m_shapesVB;
	ID3D11Buffer *m_shapesIB;
	ID3D11Buffer *m_skullVB;
	ID3D11Buffer *m_skullIB;
};

#endif