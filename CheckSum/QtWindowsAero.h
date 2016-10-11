#ifndef QTWINDOWSAERO_H
#define QTWINDOWSAERO_H

#include <QWidget>
#include <QColor>

class WindowNotifier;

class QtWindowsAero
{
public:
	QtWindowsAero();
	~QtWindowsAero();

	static bool enableAeroWindow(QWidget *widget, bool enable = true);  
    static bool extendFrameIntoClientArea(QWidget *widget,  
                                          int left = -1, int top = -1,  
                                          int right = -1, int bottom = -1);  
    static bool isCompositionEnabled();  
    static QColor colorizatinColor();  

private:
	static WindowNotifier *windowNotifier();  
};

#endif // QTWINDOWSAERO_H
