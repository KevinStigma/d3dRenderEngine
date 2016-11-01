#include "d3drenderengine.h"
#include "RenderWidget.h"
#include "GlobalSys.h"
#include <QString>
#include <QFileDialog>
#include<QKeyEvent>
#include <iostream>

d3dRenderEngine::d3dRenderEngine(QWidget *parent)
	: QMainWindow(parent)
{
	g_pGlobalSys = new GlobalSys;
	ui.setupUi(this);
}

d3dRenderEngine::~d3dRenderEngine()
{
	SAFE_DELETE(g_pGlobalSys);
}

void d3dRenderEngine::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_L:
	{
					  ui.render_widget->testArcball();
					  break;
	}
	}
}

void d3dRenderEngine::on_actionLoad_Obj_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Import obj"), "../Data/", "obj files(*.obj)", 0);
	if (!filename.size())
		return;
	g_pGlobalSys->loadObj(filename.toStdString());
	g_pGlobalSys->genNormals();
	ui.render_widget->loadObjData();
}
