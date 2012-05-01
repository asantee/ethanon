#!/bin/sh

die() {
  echo "Aborting..."
  exit 1
}

rm -rf dist       || die

./bootstrap       || die
mkdir -p dist     || die
cd dist           || die
../configure      || die
make dist         || die
GZFILE=`ls *.tar.gz`
BZFILE=`echo $GZFILE | sed s/\\.tar\\.gz/.tar.bz2/`
(zcat $GZFILE | bzip2 > $BZFILE) || die

SRCFILE=`echo $GZFILE | sed s/\\.tar\\.gz/-src.tbz/`
cvs -d `cat ../CVS/Root` export -D now audiere || die
tar cfj $SRCFILE audiere || die
