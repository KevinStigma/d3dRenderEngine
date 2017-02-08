#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H
#include <QWidget>
#include "GameTimer.h"
#include "Arcball/MyArcball.h"

class D3DApp;
enum Scene{HILLWAVE,MIRROR,TESSELLATION,ALTAR,CUBEMAP,NORMAL_DISPMAP,TERRAIN,PARTICLE,SHADOW,SSAO,ANIMATION};
class RenderWidget:public QWidget
{
	Q_OBJECT
public:
	RenderWidget(QWidget *parent = 0);
	~RenderWidget();
	void loadObjData();
	void loadScene(Scene s);
	virtual QPaintEngine *paintEngine() const
	{
		return NULL;
	}

	virtual void resizeEvent(QResizeEvent *event); 
	virtual void paintEvent(QPaintEvent *event);   
	
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
private:
	
	D3DApp* m_d3dApp;
	GameTimer m_timer;
	int m_frameCount;
	MyArcball*m_arcball;
	bool m_firstStart;
	Scene m_curScene;
};

#endif