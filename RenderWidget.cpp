#include "RenderWidget.h"
#include <assert.h>
#include <QKeyEvent>
#include <QMessageBox>
#include <iostream>

RenderWidget::RenderWidget(QWidget*parent):QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);
	m_d3dApp.initD3D((HWND)winId(),width(), height());
	m_d3dApp.initScene(width(), height());
	m_frameCount = 0;
	m_timer.Reset();
}

RenderWidget::~RenderWidget()
{
}

void RenderWidget::resizeEvent(QResizeEvent *event)
{
	m_d3dApp.resizeD3D(width(), height());
}
void RenderWidget::paintEvent(QPaintEvent *event)
{
	//compute fps
	m_frameCount++;
	m_timer.Tick();
	float elapsedTime = m_timer.TotalTime();
	if (elapsedTime > 1.0f)
	{
		topLevelWidget()->setWindowTitle("FPS: " + QString::number(m_frameCount));
		m_frameCount = 0;
		m_timer.Reset();
	}
	m_d3dApp.updateScene(m_timer.DeltaTime()*2);
	m_d3dApp.renderScene();

	update();
}

void RenderWidget::keyPressEvent(QKeyEvent *event)
{

}
void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{
}
void RenderWidget::mousePressEvent(QMouseEvent *event)
{

}
void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
	
}
void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{

}
void RenderWidget::wheelEvent(QWheelEvent *event)
{

}