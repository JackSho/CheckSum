#include <QMouseEvent>

#include "MoveLabel.h"

MoveLabel::MoveLabel(QWidget *parent)
	: QLabel(parent),press(false)
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

MoveLabel::~MoveLabel()
{

}

void MoveLabel::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		press = true;
		relativePos=parentWidget->pos()-event->globalPos();
	}
}
void MoveLabel::mouseMoveEvent(QMouseEvent *event)
{
	if(press)
		parentWidget->move(event->globalPos()+ relativePos);
}

void MoveLabel::mouseReleaseEvent(QMouseEvent *event)
{
	press = false;
}