#include <stdlib.h>
#include <stdio.h>
#include "types.h"

#ifdef DEBUGMODE

#ifndef ASSERT
#include <assert.h>
#define ASSERT(n) assert(n)
#endif
#ifndef TRACE
// it would be nice if this did actually trace ...
#define TRACE 1 ? (void)0 : (void)printf
#endif

#else

#ifndef ASSERT
#define ASSERT(n)
#endif
#ifndef TRACE
#define TRACE 1 ? (void)0 : (void)printf
#endif

#endif

typedef audiere::s64 LONG_LONG;


namespace audiere {

typedef int sample_t;

#define DUMB_RQ_ALIASING 0
#define DUMB_RQ_LINEAR   1
#define DUMB_RQ_CUBIC    2
#define DUMB_RQ_N_LEVELS 3
extern int dumb_resampling_quality;

typedef struct DUMB_RESAMPLER DUMB_RESAMPLER;

typedef void (*DUMB_RESAMPLE_PICKUP)(DUMB_RESAMPLER *resampler, void *data);

struct DUMB_RESAMPLER
{
	sample_t *src;
	long pos;
	int subpos;
	long start, end;
	int dir;
	DUMB_RESAMPLE_PICKUP pickup;
	void *pickup_data;
	int min_quality;
	int max_quality;
	/* Everything below this point is internal: do not use. */
	sample_t x[3];
	int overshot;
};

void dumb_reset_resampler(DUMB_RESAMPLER *resampler, sample_t *src, long pos, long start, long end);
DUMB_RESAMPLER *dumb_start_resampler(sample_t *src, long pos, long start, long end);
long dumb_resample(DUMB_RESAMPLER *resampler, sample_t *dst, long dst_size, float volume, float delta);
sample_t dumb_resample_get_current_sample(DUMB_RESAMPLER *resampler, float volume);
void dumb_end_resampler(DUMB_RESAMPLER *resampler);

}
