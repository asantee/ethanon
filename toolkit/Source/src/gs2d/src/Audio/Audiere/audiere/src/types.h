#ifndef AUDIERE_TYPES_H
#define AUDIERE_TYPES_H

namespace audiere {

  #ifdef _MSC_VER  // VC++

    typedef unsigned __int8  u8;
    typedef signed   __int8  s8;
    typedef unsigned __int16 u16;
    typedef signed   __int16 s16;
    typedef unsigned __int32 u32;
    typedef signed   __int32 s32;
    typedef unsigned __int64 u64;
    typedef signed   __int64 s64;

  #else            // assume gcc i386 linux ;)

    typedef unsigned char      u8;
    typedef signed   char      s8;
    typedef unsigned short     u16;
    typedef signed   short     s16;
    typedef unsigned long      u32;
    typedef signed   long      s32;
    typedef unsigned long long u64;
    typedef signed   long long s64;

  #endif

}

#endif
