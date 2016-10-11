#ifndef MOVELABEL_H
#define MOVELABEL_H

#include <QLabel>

class MoveLabel : public QLabel
{
public:
	MoveLabel(QWidget *parent);
	~MoveLabel();

private:

	QWidget *parentWidget;
	bool press;
	QPoint relativePos;
	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MOVELABEL_H
