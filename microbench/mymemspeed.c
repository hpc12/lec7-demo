#include <stdio.h>
#include <stdlib.h>

#define MS_COPY         1
#define MS_WRITE        2
#define MS_READ         3

typedef unsigned long ulong;
ulong st_low, st_high;
ulong end_low, end_high;
ulong cal_low, cal_high;
int tsc_invariable = 0;

#if 0

// Marsha's desktop
double clks_msec = 3.33e6;
int l1_cache = 32; // kiB
int l2_cache = 256;
int l3_cache = 12*1024;

#else

// Andreas' laptop
double clks_msec = 1.73e6;
int l1_cache = 32; // kiB
int l2_cache = 2048;
int l3_cache = 0;

#endif


/* Measure cache/memory speed by copying a block of memory. */
/* Returned value is kbytes/second */
double memspeed(char *src, ulong len, int iter, int type)
{
        char *dst;
        ulong wlen;
        int i;

        dst = src + len;
        wlen = len / 4;  /* Length is bytes */

        /* Calibrate the overhead with a zero word copy */
        asm __volatile__ ("rdtsc":"=a" (st_low),"=d" (st_high));
        for (i=0; i<iter; i++) {
                asm __volatile__ (
                        "movl %0,%%esi\n\t" \
                        "movl %1,%%edi\n\t" \
                        "movl %2,%%ecx\n\t" \
                        "cld\n\t" \
                        "rep\n\t" \
                        "movsl\n\t" \
                                :: "g" (src), "g" (dst), "g" (0)
                        : "esi", "edi", "ecx"
                );
        }
        asm __volatile__ ("rdtsc":"=a" (cal_low),"=d" (cal_high));

        /* Compute the overhead time */
        asm __volatile__ (
                "subl %2,%0\n\t"
                "sbbl %3,%1"
                :"=a" (cal_low), "=d" (cal_high)
                :"g" (st_low), "g" (st_high),
                "0" (cal_low), "1" (cal_high)
        );


        /* Now measure the speed */
        switch (type) {
        case MS_COPY:
                /* Do the first copy to prime the cache */
                asm __volatile__ (
                        "movl %0,%%esi\n\t" \
                        "movl %1,%%edi\n\t" \
                        "movl %2,%%ecx\n\t" \
                        "cld\n\t" \
                        "rep\n\t" \
                        "movsl\n\t" \
                        :: "g" (src), "g" (dst), "g" (wlen)
                        : "esi", "edi", "ecx"
                );
                asm __volatile__ ("rdtsc":"=a" (st_low),"=d" (st_high));
                for (i=0; i<iter; i++) {
                        asm __volatile__ (
                                "movl %0,%%esi\n\t" \
                                "movl %1,%%edi\n\t" \
                                "movl %2,%%ecx\n\t" \
                                "cld\n\t" \
                                "rep\n\t" \
                                "movsl\n\t" \
                                :: "g" (src), "g" (dst), "g" (wlen)
                                : "esi", "edi", "ecx"
                        );
                }
                asm __volatile__ ("rdtsc":"=a" (end_low),"=d" (end_high));
                break;
        case MS_WRITE:
                asm __volatile__ ("rdtsc":"=a" (st_low),"=d" (st_high));
                for (i=0; i<iter; i++) {
                        asm __volatile__ (
                                "movl %0,%%ecx\n\t" \
                                "movl %1,%%edi\n\t" \
                                "movl %2,%%eax\n\t" \
                                "rep\n\t" \
                                "stosl\n\t"
                                :: "g" (wlen), "g" (dst), "g" (0)
                                : "edi", "ecx", "eax"
                        );
                }
                asm __volatile__ ("rdtsc":"=a" (end_low),"=d" (end_high));
                break;
        case MS_READ:
                asm __volatile__ (
                        "movl %0,%%esi\n\t" \
                        "movl %1,%%ecx\n\t" \
                        "cld\n\t" \
                        "L1:\n\t" \
                        "lodsl\n\t" \
                        "loop L1\n\t" \
                        :: "g" (src), "g" (wlen)
                        : "esi", "ecx"
                );
                asm __volatile__ ("rdtsc":"=a" (st_low),"=d" (st_high));
                for (i=0; i<iter; i++) {
                        asm __volatile__ (
                                "movl %0,%%esi\n\t" \
                                "movl %1,%%ecx\n\t" \
                                "cld\n\t" \
                                "L2:\n\t" \
                                "lodsl\n\t" \
                                "loop L2\n\t" \
                                :: "g" (src), "g" (wlen)
                                : "esi", "ecx", "eax"
                        );
                }
                asm __volatile__ ("rdtsc":"=a" (end_low),"=d" (end_high));
                break;
        }

        /* Compute the elapsed time */
        asm __volatile__ (
                "subl %2,%0\n\t"
                "sbbl %3,%1"
                :"=a" (end_low), "=d" (end_high)
                :"g" (st_low), "g" (st_high),
                "0" (end_low), "1" (end_high)
        );
        /* Subtract the overhead time */
        asm __volatile__ (
                "subl %2,%0\n\t"
                "sbbl %3,%1"
                :"=a" (end_low), "=d" (end_high)
                :"g" (cal_low), "g" (cal_high),
                "0" (end_low), "1" (end_high)
        );

        /* Make sure that the result fits in 32 bits */
        if (end_high) {
                return(0);
        }

        /* If this was a copy adjust the time */
        if (type == MS_COPY) {
                end_low /= 2;
        }

        /* Convert to clocks/KB */
        end_low /= len;
        end_low *= 1024;
        end_low /= iter;
        if (end_low == 0) {
                return(0);
        }

        // if(tsc_invariable){ end_low = correct_tsc(end_low);  }

        /* Convert to kbytes/sec */
        return(clks_msec/end_low);
}

int main()
{
  char *memarea = malloc(1 << 28);

  double l1_speed = memspeed(memarea, (l1_cache / 4) * 1024, 200, MS_COPY);
  printf("L1 speed: %g MB/s\n", l1_speed);
  double l2_speed = memspeed(memarea, l1_cache * 1024, 200, MS_COPY);
  printf("L2 speed: %g MB/s\n", l2_speed);
  if (l3_cache > 0)
  {
    double l3_speed = memspeed(memarea, l2_cache * 1024, 200, MS_COPY);
    printf("L3 speed: %g MB/s\n", l3_speed);
  }
  double mem_speed = memspeed(memarea, (l1_cache+l2_cache+l3_cache)*5 * 1024, 50, MS_COPY);
  printf("Mem speed: %g MB/s\n", mem_speed);
  return 0;
}
