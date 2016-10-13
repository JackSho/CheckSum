#!/bin/sh

echo "arg num is $#"
echo "arg0 is $0"
echo "arg1 is $1"
TRAGEFULLPATH=$1
EXECTABLENAME=CheckSum.app
EXECTABLEFULLDIR=./Debug/${EXECTABLENAME}/Contents/MacOS


echo "finish !! finish !! finish!!${TARGELIBFULLDIR}"
echo "${TARGELIBFULLDIR}"
TARGELIBFULLDIR=${TRAGEFULLPATH}/Contents/MacOS/lib

if [ ! -d "${TARGELIBFULLDIR}" ]
then
mkdir "${TARGELIBFULLDIR}"
fi


LIBQTORIGALINSTALLPATH=/usr/local/Qt-5.2.1/lib/

LIBINSTALLPATH=@executable_path/lib/

install_name_tool -change ${LIBQTORIGALINSTALLPATH}QtNetwork.framework/Versions/5/QtNetwork	  ${LIBINSTALLPATH}QtNetwork   ${TRAGEFULLPATH}
install_name_tool -change ${LIBQTORIGALINSTALLPATH}QtCore.framework/Versions/5/QtCore  ${LIBINSTALLPATH}QtCore   ${TRAGEFULLPATH}
install_name_tool -change ${LIBQTORIGALINSTALLPATH}QtGui.framework/Versions/5/QtGui  ${LIBINSTALLPATH}QtGui   ${TRAGEFULLPATH}
install_name_tool -change ${LIBQTORIGALINSTALLPATH}QtWidgets.framework/Versions/5/QtWidgets  ${LIBINSTALLPATH}QtWidgets   ${TRAGEFULLPATH}

cp -r ${LIBQTORIGALINSTALLPATH}QtNetwork.framework/Versions/5/QtNetwork ${TARGELIBFULLDIR}/QtNetwork
cp -r ${LIBQTORIGALINSTALLPATH}QtCore.framework/Versions/5/QtCore ${TARGELIBFULLDIR}/QtCore
cp -r ${LIBQTORIGALINSTALLPATH}QtGui.framework/Versions/5/QtGui ${TARGELIBFULLDIR}/QtGui
cp -r ${LIBQTORIGALINSTALLPATH}QtWidgets.framework/Versions/5/QtWidgets ${TARGELIBFULLDIR}/QtWidgets
