#!/bin/sh

HHC="C:/Program Files/HTML Help Workshop/hhc"

die() {
    echo "Aborting..."
    exit 1
}

archive() {
    VERSION=1.9.4
    AUDIENCE=$1
    BASE=audiere-$VERSION-$AUDIENCE-doxygen

    rm -rf "$AUDIENCE" || die
    doxygen audiere-$AUDIENCE.doxy || die
    (cd "$AUDIENCE" && \
     cp -r html $BASE && \
     zip -r -q ../$BASE.zip $BASE && \
     tar cfj ../$BASE.tar.bz2 $BASE) || die

    (cd $AUDIENCE/html && "$HHC" index.hhp)
    mv $AUDIENCE/html/index.chm $BASE.chm || die
}

archive users
archive devel
