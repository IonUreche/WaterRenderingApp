#pragma once

#include <QtWidgets/QMainWindow>
#include "GUI/GeneratedFiles/ui_MainWindow.h"
#include <QKeyEvent>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	//void MainWindow::keyPressEvent(QKeyEvent *event);

private:
	Ui::MainWindow ui;
};
