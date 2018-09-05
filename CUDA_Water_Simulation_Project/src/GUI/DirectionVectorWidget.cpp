#include "DirectionVectorWidget.h"
#include "GUI/GeneratedFiles/ui_DirectionVectorWidget.h"
#include "GUI/GeneratedFiles/moc_DirectionVectorWidget.cpp"

DirectionVectorWidget::DirectionVectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectionVectorWidget)
{
    ui->setupUi(this);
	setFixedSize(100, 100);
}
////////////////////////////////////////////////////////////////////////////
DirectionVectorWidget::~DirectionVectorWidget()
{
	delete ui;
}
////////////////////////////////////////////////////////////////////////////
void DirectionVectorWidget::mousePressEvent(QMouseEvent *event)
{
	//QWidget::mousePressEvent(event);
	m_lastClickPos = event->pos();

	QLineF l(width() / 2, height() / 2, m_lastClickPos.x(), m_lastClickPos.y());
	l = l.unitVector();
	normDir = l.p2() - l.p1();

	emit onMousePressed2(normDir.x(), normDir.y());
	update();
}
////////////////////////////////////////////////////////////////////////////
void DirectionVectorWidget::mouseMoveEvent(QMouseEvent *event)
{
	//QWidget::mouseMoveEvent(event);
	m_lastClickPos = event->pos();

	QLineF l(width() / 2, height() / 2, m_lastClickPos.x(), m_lastClickPos.y());
	l = l.unitVector();
	normDir = l.p2() - l.p1();

	emit onMousePressed2(normDir.x(), normDir.y());
	update();
}
////////////////////////////////////////////////////////////////////////////
void DirectionVectorWidget::mouseReleaseEvent(QMouseEvent *event)
{
	//QWidget::mouseReleaseEvent(event);
	m_lastClickPos = event->pos();

	QLineF l(width() / 2, height() / 2, m_lastClickPos.x(), m_lastClickPos.y());
	l = l.unitVector();
	normDir = l.p2() - l.p1();

	emit onMousePressed2(normDir.x(), normDir.y());
	update();
}
////////////////////////////////////////////////////////////////////////////
void DirectionVectorWidget::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);

	QPainter p(this);
	QPen pen(Qt::black, 1);
	p.setPen(pen);
	QBrush br(Qt::lightGray, Qt::BrushStyle::SolidPattern);
	p.setBrush(br);

	p.drawRect(0, 0, width(), height());

	pen.setColor(Qt::green);
	p.setPen(pen);

	QVector<float> v;
	//v.normali
	//l.norm

	QLineF l(width() / 2, height() / 2, m_lastClickPos.x(), m_lastClickPos.y());
	l = l.unitVector();
	normDir = l.p2() - l.p1();
	l.setP2(l.p1() + (l.p2() - l.p1()) * width() / 2);
	//l.setP1(QPointF(0, 0));

	p.drawLine(l.p1(), l.p2());
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
