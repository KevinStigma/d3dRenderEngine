#ifndef D3DRENDERENGINE_H
#define D3DRENDERENGINE_H

#include <QtWidgets/QMainWindow>
#include "ui_d3drenderengine.h"

class d3dRenderEngine : public QMainWindow
{
	Q_OBJECT

public:
	d3dRenderEngine(QWidget *parent = 0);
	~d3dRenderEngine();
	void keyPressEvent(QKeyEvent *event);

public slots:
	void on_actionHillWaves_triggered();
	void on_actionMirror_triggered();
	void on_actionTessellation_triggered();
	void on_actionAltar_triggered();
	void on_actionCubeMap_triggered();
	void on_actionNor_DispMap_triggered();
	void on_actionTerrain_triggered();
	void on_actionParticle_triggered();
	void on_actionShadow_triggered();
	void on_actionSSAO_triggered();
	void on_actionAnimation_triggered();
private:
	void updateActionsChecked(QAction* cur_action);
	void loadObj();
	std::vector<QAction*> sceneActions;
	Ui::d3dRenderEngineClass ui;
};

#endif // D3DRENDERENGINE_H
