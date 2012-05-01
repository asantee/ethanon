This directory contains headers and precompiled VC6 libraries for
external libraries, such as DUMB, FLAC, Ogg Vorbis, and Python, that
Audiere depends on.  The files in this directory should also work when
using the VC7 build system.

For use in the core Audiere DLL, libraries should be built statically
using the /MT (release, multithreaded) and /MTd (debug, multithreaded)
options.  audiere.dll should not depend on any external, nonsystem
DLLs.


Version dependencies of third-party software should be maintained in
this file:

DUMB (CVS from 2003.06.12)
libogg 1.0
libvorbis 1.0
libFLAC 1.1.0
libFLAC++ 1.1.0
