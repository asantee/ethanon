#!/bin/sh

die() {
    echo "Error, aborting..."
    exit 1
}

VERSION=1.9.4
NAME=audiere-$VERSION-sgi
FILE=$NAME.tar.gz

scons prefix=dist/$NAME dist/$NAME || die
cd dist || die
find . -name .sconsign | xargs rm -f
rm -f $FILE || die
/usr/freeware/bin/tar cfz $FILE $NAME || die

echo
echo "SGI release built: dist/${FILE}"
