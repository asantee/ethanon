#include <time.h>
#include <sys/time.h>
#include "timer.h"
#include "types.h"


namespace audiere {

  u64 GetNow() {

#if HAVE_CLOCK_GETTIME
    // use the POSIX realtime clock to get the current time
    struct timespec tp;
    int result = clock_gettime(CLOCK_REALTIME, &tp);
    if (result == 0) {
      return u64(tp.tv_sec) * 1000000 + u64(tp.tv_nsec) / 1000;
    }
#endif

    // can't use realtime clock!  Try to use gettimeofday
    struct timeval tv;
    gettimeofday(&tv, 0);
    return u64(tv.tv_sec) * 1000000 + tv.tv_usec;
  }

}
