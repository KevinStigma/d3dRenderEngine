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
	sceneActions.push_back(ui.actionAltar);
	sceneActions.push_back(ui.actionAnimation);
	sceneActions.push_back(ui.actionCubeMap);
	sceneActions.push_back(ui.actionHillWaves);
	sceneActions.push_back(ui.actionMirror);
	sceneActions.push_back(ui.actionNor_DispMap);
	sceneActions.push_back(ui.actionParticle);
	sceneActions.push_back(ui.actionShadow);
	sceneActions.push_back(ui.actionSSAO);
	sceneActions.push_back(ui.actionTerrain);
	sceneActions.push_back(ui.actionTessellation);
}

void d3dRenderEngine::updateActionsChecked(QAction* cur_action)
{
	for (int i = 0; i < (int)sceneActions.size(); i++)
	{
		if (cur_action != sceneActions[i])
			sceneActions[i]->setChecked(false);
	}
}

d3dRenderEngine::~d3dRenderEngine()
{
	SAFE_DELETE(g_pGlobalSys);
}

void d3dRenderEngine::keyPressEvent(QKeyEvent *event)
{
}

void d3dRenderEngine::loadObj()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Import obj"), "./Data/", "obj files(*.obj)", 0);
	if (!filename.size())
		return;
	g_pGlobalSys->loadObj(filename.toStdString());
	g_pGlobalSys->genNormals();
	ui.render_widget->loadObjData();
}

void d3dRenderEngine::on_actionHillWaves_triggered()
{
	updateActionsChecked(ui.actionHillWaves);
	ui.render_widget->loadScene(HILLWAVE);
}
void d3dRenderEngine::on_actionMirror_triggered()
{
	updateActionsChecked(ui.actionMirror);
	ui.render_widget->loadScene(MIRROR);
}
void d3dRenderEngine::on_actionTessellation_triggered()
{
	updateActionsChecked(ui.actionTessellation);
	ui.render_widget->loadScene(TESSELLATION);
}
void d3dRenderEngine::on_actionAltar_triggered()
{
	updateActionsChecked(ui.actionAltar);
	ui.render_widget->loadScene(ALTAR);
}
void d3dRenderEngine::on_actionCubeMap_triggered()
{
	updateActionsChecked(ui.actionCubeMap);
	ui.render_widget->loadScene(CUBEMAP);
}
void d3dRenderEngine::on_actionNor_DispMap_triggered()
{
	updateActionsChecked(ui.actionNor_DispMap);
	ui.render_widget->loadScene(NORMAL_DISPMAP);
}
void d3dRenderEngine::on_actionTerrain_triggered()
{
	updateActionsChecked(ui.actionTerrain);
	ui.render_widget->loadScene(TERRAIN);
}
void d3dRenderEngine::on_actionParticle_triggered()
{
	updateActionsChecked(ui.actionParticle);
	ui.render_widget->loadScene(PARTICLE);
}
void d3dRenderEngine::on_actionShadow_triggered()
{
	updateActionsChecked(ui.actionShadow);
	ui.render_widget->loadScene(SHADOW);
}
void d3dRenderEngine::on_actionSSAO_triggered()
{
	updateActionsChecked(ui.actionSSAO);
	ui.render_widget->loadScene(SSAO);
}
void d3dRenderEngine::on_actionAnimation_triggered()
{
	updateActionsChecked(ui.actionAnimation);
	ui.render_widget->loadScene(ANIMATION);
}
