#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QEvent>

class DirectionVectorWidget : public QWidget
{
	//Q_OBJECT
public:
	DirectionVectorWidget(QWidget* parent = nullptr);
	~DirectionVectorWidget();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void paintEvent(QPaintEvent *event) override;

//signals:
//	void onMousePressed2(float x, float y);

private:
	QPoint m_lastClickPos;

	QPointF normDir;
};