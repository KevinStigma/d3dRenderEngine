#ifndef TESSELLATIONAPP_H
#define TESSELLATIONAPP_H
#include <QMouseEvent>
#include "D3DApp.h"

class TessellationApp :public D3DApp
{
public:
	TessellationApp();
	bool initD3D(HWND windowId, int width, int height);
	void initScene(int width, int height);
	void updateScene(GameTimer*gameTimer);
	void renderScene();
	void keyPressEvent(QKeyEvent* event);
	void cleanUp();
protected:
	void buildQuadPatchBuffer();

	ID3D11Buffer* m_quadPatchVB;
	float m_theta;
	float m_phi;
	float m_radius;
	QPoint m_lastPoint;
};
#endif