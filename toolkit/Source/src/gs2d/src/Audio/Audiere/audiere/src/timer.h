/**
 * @file
 *
 * Platform-independent abstraction for high-resolution timers.
 */


#ifndef TIMER_H
#define TIMER_H


#include "types.h"


namespace audiere {

  /// Return current time in microseconds.
  u64 GetNow();

}


#endif
