#include "RenderWidget.h"
#include <assert.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include <iostream>
#include "foundation.h"
#include "D3DApp/D3DApp.h"
#include "D3DApp/CrateApp.h"
#include "D3DApp/HillWaveApp.h"
#include "D3DApp/MirrorApp.h"


RenderWidget::RenderWidget(QWidget*parent) :QWidget(parent), m_firstStart(true)
{
	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);
	
	m_d3dApp = new MirrorApp;
	m_frameCount = 0;
	m_timer.Reset();

}

RenderWidget::~RenderWidget()
{
	SAFE_DELETE(m_arcball);
	m_d3dApp->cleanUp();
	SafeDelete(m_d3dApp);
}


void RenderWidget::testArcball()
{
	m_arcball->MousePt.s.X = 600;
	m_arcball->MousePt.s.Y = 299;

	m_arcball->LastRot = m_arcball->ThisRot;
	m_arcball->ArcBall.click(&m_arcball->MousePt);

	m_arcball->MousePt.s.X = 200;
	m_arcball->MousePt.s.Y = 299;
	Quat4fT     ThisQuat;
	m_arcball->ArcBall.drag(&m_arcball->MousePt, &ThisQuat);
	
	Matrix3fSetRotationFromQuat4f(&m_arcball->ThisRot, &ThisQuat);
	m_arcball->ThisRot = Matrix3fMulMatrix3f(&m_arcball->LastRot, &m_arcball->ThisRot);
	Matrix4fSetRotationFromMatrix3f(&m_arcball->Transform, &m_arcball->ThisRot);
}

void RenderWidget::resizeEvent(QResizeEvent *event)
{
	if (m_firstStart)
	{
		m_d3dApp->initD3D((HWND)winId(), width(), height());
		m_d3dApp->initScene(width(), height());
		m_arcball = new MyArcball(width(), height());
		//testArcball();
		m_firstStart = false;
	}
	m_d3dApp->resizeD3D(width(), height());
}
void RenderWidget::paintEvent(QPaintEvent *event)
{
	//compute fps
	m_frameCount++;
	m_timer.Tick();
	static float timeElapsed = 0.0f;

	if (m_timer.TotalTime()-timeElapsed > 1.0f)
	{
		topLevelWidget()->setWindowTitle("FPS: " + QString::number(m_frameCount));
		m_frameCount = 0;
		timeElapsed += 1.0f;
	}
	m_d3dApp->updateScene(&m_timer);
	m_d3dApp->setTransMat(m_arcball->Transform.M);
	m_d3dApp->renderScene();
	update();
}

void RenderWidget::loadObjData()
{
	m_d3dApp->loadObjData();
}

void RenderWidget::keyPressEvent(QKeyEvent *event)
{
}
void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{
}
void RenderWidget::mousePressEvent(QMouseEvent *mouse_event)
{
	QPoint qpoint = mapFromGlobal(QCursor::pos());
	int posx = qpoint.x();
	int posy = qpoint.y();
	if (mouse_event->button() == Qt::LeftButton)
	{
		m_arcball->MousePt.s.X = posx;
		m_arcball->MousePt.s.Y = posy;

		m_arcball->LastRot = m_arcball->ThisRot;
		m_arcball->ArcBall.click(&m_arcball->MousePt);
		m_arcball->button_status = 1;
	}
	else if (mouse_event->button() == Qt::RightButton)
		m_arcball->button_status = 2;
	m_arcball->mLastMousePos.s.X = static_cast<LONG>(qpoint.x());
	m_arcball->mLastMousePos.s.Y = static_cast<LONG>(qpoint.y());
	setMouseTracking(true);
	grabMouse();
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
	releaseMouse();
	setMouseTracking(false);
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPoint qpoint = mapFromGlobal(QCursor::pos());
	int posx = qpoint.x();
	int posy = qpoint.y();
	if (m_arcball->button_status == 1)//rotate
	{
		m_arcball->MousePt.s.X = posx;
		m_arcball->MousePt.s.Y = posy;
		Quat4fT     ThisQuat;
		m_arcball->ArcBall.drag(&m_arcball->MousePt, &ThisQuat);
		Matrix3fSetRotationFromQuat4f(&m_arcball->ThisRot, &ThisQuat);
		m_arcball->ThisRot = Matrix3fMulMatrix3f(&m_arcball->LastRot, &m_arcball->ThisRot);
		Matrix4fSetRotationFromMatrix3f(&m_arcball->Transform, &m_arcball->ThisRot);
	}
	else if (m_arcball->button_status == 2)//translate
	{
		float dx = 0.01f*static_cast<float>(qpoint.x() - m_arcball->mLastMousePos.s.X);
		float dy = 0.01f*static_cast<float>(qpoint.y() - m_arcball->mLastMousePos.s.Y);
		m_arcball->translate_x += dx;
		m_arcball->translate_y -= dy;
		m_d3dApp->setTranslate(m_arcball->translate_x, m_arcball->translate_y);
	}
	m_arcball->mLastMousePos.s.X = posx;
	m_arcball->mLastMousePos.s.Y = posy;
}
void RenderWidget::wheelEvent(QWheelEvent *event)
{
	float delta = (float)event->delta() / 3000.0f;
	Matrix4fMulRotationScale(&m_arcball->Transform, (1 + delta));
}