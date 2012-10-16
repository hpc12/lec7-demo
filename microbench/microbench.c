#if 0

# Just mark executable and run
# The shell (at least bash and zsh) will do the right thing.

set -e

gcc-4.6 -std=c99 -pthread -fno-strict-aliasing -fwrapv -Wall -O3 -DNDEBUG -fPIC -O3 -o /tmp/executable $0

/tmp/executable
exit 0

#else

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

double timeval_subtract(struct timeval *x, struct timeval *y)
{
  struct timeval result;

  /* Perform the carry for the later subtraction by updating Y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     `tv_usec' is certainly positive. */
  result.tv_sec = x->tv_sec - y->tv_sec;
  result.tv_usec = x->tv_usec - y->tv_usec;

  return result.tv_sec + 1e-6 * result.tv_usec;
}




void measure_strides_inner(int *ary, unsigned arr_size, unsigned count, unsigned stride)
{
  for (unsigned i = 0; i < arr_size; i += stride)
    ary[i] *= 17;
}

void measure_strides(unsigned count, unsigned stride)
{
  const unsigned arr_size = 64 * 1024 * 1024;
  int *ary = (int *) malloc(sizeof(int) * arr_size);

  for (unsigned it = 0; it < count; ++it)
  {
    measure_strides_inner(ary, arr_size, count, stride);
  }

  free(ary);
}




void measure_cache_bandwidth_inner(int*ary, unsigned array_size, unsigned steps)
{
  unsigned asm1 = array_size - 1;

  for (unsigned i = 0; i < steps; ++i)
    ary[(i*16) & asm1] ++;
}




void measure_cache_bandwidth(unsigned array_size, unsigned steps)
{
  int *ary = (int *) malloc(sizeof(int) * array_size);

  measure_cache_bandwidth_inner(ary, array_size, steps);

  free(ary);
}




int main()
{
  puts("measuring strides");

  puts("stride\ttime");
  for (unsigned stride = 1; stride < (1<<11); stride <<= 1)
  {
    struct timeval start;
    gettimeofday(&start, NULL);

    measure_strides(10, stride);

    struct timeval stop;
    gettimeofday(&stop, NULL);

    double elapsed = timeval_subtract(&stop, &start);

    printf("%d\t%g\n", stride, elapsed);
  }

  puts("measuring cache bandwidths");

  puts("size\tbandwidth\n");

  for (unsigned array_size  = 1024; array_size < (1<<26); array_size <<= 1)
  {
    unsigned steps = 1<<27;

    struct timeval start;
    gettimeofday(&start, NULL);

    measure_cache_bandwidth(array_size, steps);

    struct timeval stop;
    gettimeofday(&stop, NULL);

    double elapsed = timeval_subtract(&stop, &start);
    double gb_transferred = 2*steps*4/1e9; // 2 for rw, 4 for sizeof(int)
    double bandwidth = gb_transferred/elapsed;

    printf("%d\t%g\n", array_size, bandwidth);
  }
}
#endif
