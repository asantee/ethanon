#!/bin/sh

die() {
    echo "Error, aborting..."
    exit 1
}

vc6die() {
    echo "Error building audiere.dsw"
    echo "See build log: vc6-build.log"
    exit 1
}

echo
echo "Building Java bindings..."
echo

cd bindings/java || die
scons || die
cd ../.. || die

echo
echo "Building XPCOM bindings..."
echo

cd bindings/xpcom || die
scons || die
cd ../.. || die

echo
echo "Building Audiere VC6 workspace..."

msdev vc6/audiere.dsw /MAKE ALL /OUT vc6-build.log || vc6die

echo
echo "Build complete"
echo

NAME=audiere-1.9.4-win32
DIST=dist

scons -f vc6/dist.py prefix=$DIST/$NAME || die
find . -name .sconsign | xargs rm -f

rm -f $DIST/$NAME.zip
(cd $DIST && zip -r $NAME.zip $NAME) || die

echo
echo "Win32 release completed"
