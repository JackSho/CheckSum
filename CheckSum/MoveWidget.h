#ifndef MOVEWIDGET_H
#define MOVEWIDGET_H

#include <QWidget>

class MoveWidget : public QWidget
{
public:
	MoveWidget(QWidget *parent);
	~MoveWidget();

private:
	
	QWidget *parentWidget;
	bool press;
	QPoint relativePos;
	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MOVEWIDGET_H
