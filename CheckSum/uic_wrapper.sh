#!/bin/sh
DYLD_LIBRARY_PATH=/Users/jie/My/Study/Qt/qt-everywhere-src-5.11.0/qtbase/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}
export DYLD_LIBRARY_PATH
QT_PLUGIN_PATH=/Users/jie/My/Study/Qt/qt-everywhere-src-5.11.0/qtbase/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec /Users/jie/My/Study/Qt/qt-everywhere-src-5.11.0/qtbase/bin/uic "$@"
