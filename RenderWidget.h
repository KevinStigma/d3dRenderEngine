#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H
#include <QWidget>
#include "D3DApp.h"
#include "GameTimer.h"
#include "Arcball/MyArcball.h"

class RenderWidget:public QWidget
{
	Q_OBJECT
public:
	RenderWidget(QWidget *parent = 0);
	~RenderWidget();
	void loadObjData();
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
	void testArcball();
private:
	
	D3DApp m_d3dApp;
	GameTimer m_timer;
	int m_frameCount;
	MyArcball*m_arcball;
	bool m_firstStart;
};

#endif