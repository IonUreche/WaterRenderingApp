#include "MainWindow.h"
#include "moc_MainWindow.cpp"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

//void MainWindow::keyPressEvent(QKeyEvent *event)
//{
//	if (event->key() == Qt::Key_A)
//	{
//		// do your stuff here
//	}
//	//std::cout << "key pressed";
//}