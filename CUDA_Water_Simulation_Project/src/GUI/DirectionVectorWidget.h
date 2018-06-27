#ifndef DIRECTIONVECTORWIDGET_H
#define DIRECTIONVECTORWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QEvent>

namespace Ui {
class DirectionVectorWidget;
}

class DirectionVectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirectionVectorWidget(QWidget *parent = 0);
    ~DirectionVectorWidget();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void paintEvent(QPaintEvent *event) override;

signals:
	void onMousePressed2(float x, float y);

private:
    Ui::DirectionVectorWidget *ui;

	QPoint m_lastClickPos;

	QPointF normDir;
};

#endif // DIRECTIONVECTORWIDGET_H
