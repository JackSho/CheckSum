#!/bin/sh
TRAGEFULLPATH=${CONFIGURATION_BUILD_DIR}/${PRODUCT_NAME}.${EXECUTABLE_EXTENSION}

$SRCROOT/./ChangeLibrary.sh

cp -r ${TRAGEFULLPATH} $SRCROOT/../Mac/Release
