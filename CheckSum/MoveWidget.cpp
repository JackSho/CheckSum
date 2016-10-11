#include <QMouseEvent>

#include "MoveWidget.h"

MoveWidget::MoveWidget(QWidget *parent)
	: QWidget(parent),press(false)
{	
	QWidget *parentW = this;
	QWidget *parentParentW = parent;
	while(parentParentW)
	{
		parentW = parentParentW;
		parentParentW = parentParentW->parentWidget();
	}
	parentWidget = parentW;
}

MoveWidget::~MoveWidget()
{

}

void MoveWidget::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		press = true;
		relativePos=parentWidget->pos()-event->globalPos();
	}
}
void MoveWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(press && event->button() == Qt::LeftButton)
		parentWidget->move(event->globalPos()+ relativePos);
}

void MoveWidget::mouseReleaseEvent(QMouseEvent *event)
{
	press = false;
}