#!/bin/sh

die() {
    echo
    echo "Error, aborting..."
    exit 1
}


BASE=audiere-1.9.4


export CVSROOT=`cat CVS/Root`
echo
echo "Using CVSROOT: $CVSROOT"
echo

# enter 'release' directory
mkdir -p release || die
cd release || die

# 'files' is where all of the files to be uploaded go
FILES=files
mkdir -p $FILES || die

# check out the base CVS repository
rm -rf audiere || die
cvs export -D now audiere || die
cp -r audiere audiere-unix || die
cp -r audiere audiere-doxygen || die

echo
echo "Building binary release..."
case `uname` in
    CYGWIN*)
        echo "Platform: Win32"
        cp -r audiere audiere-win32 || die
        (cd audiere-win32 && ./make-release-win32.sh) || die
        cp audiere-win32/dist/$BASE-win32.zip "$FILES" || die
        ;;

    IRIX*)
        echo "Platform: SGI"
        cp -r audiere audiere-sgi || die
        (cd audiere-sgi && ./make-release-sgi.sh) || die
        cp audiere-sgi/dist/$BASE-sgi.tar.gz "$FILES" || die
        ;;

    *)
        echo "WARNING: Unknown platform, not building binary release"
        ;;
esac

echo
echo "Building UNIX release..."
echo

(cd audiere-unix && ./bootstrap && ./configure && make dist) || die
cp audiere-unix/$BASE.tar.gz "$FILES" || die

echo
echo "Building Doxygen release..."
echo

(cd audiere-doxygen/doc/doxygen && ./doxygen-dist.sh) || die
cp audiere-doxygen/doc/doxygen/$BASE-users-doxygen.chm     "$FILES" || die
cp audiere-doxygen/doc/doxygen/$BASE-users-doxygen.zip     "$FILES" || die
cp audiere-doxygen/doc/doxygen/$BASE-users-doxygen.tar.bz2 "$FILES" || die
cp audiere-doxygen/doc/doxygen/$BASE-devel-doxygen.chm     "$FILES" || die
cp audiere-doxygen/doc/doxygen/$BASE-devel-doxygen.zip     "$FILES" || die
cp audiere-doxygen/doc/doxygen/$BASE-devel-doxygen.tar.bz2 "$FILES" || die

echo
echo "Building source release..."
echo

tar cfvj "$FILES/$BASE-src.tar.bz2" audiere
