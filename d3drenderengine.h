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

private:
	Ui::d3dRenderEngineClass ui;
};

#endif // D3DRENDERENGINE_H
