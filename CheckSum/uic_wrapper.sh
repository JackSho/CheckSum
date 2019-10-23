#!/bin/sh
DYLD_LIBRARY_PATH=/Users/jie/my/study/qt/qt5/qtbase/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}
export DYLD_LIBRARY_PATH
QT_PLUGIN_PATH=/Users/jie/My/Study/Qt/qt5/qtbase/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec /Users/jie/my/study/qt/qt5/qtbase/bin/uic "$@"
