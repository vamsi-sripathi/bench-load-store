#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>

#define NTRIALS    (10)
#define ALIGN_2M   (2ULL<<20)
#define ALIGN_4K   (4ULL<<10)
#define MAX_OFFSET (512)

#include "mkl.h"
#define TIMER dsecnd

void scale (long long *p_n, double *p_scalar, double *p_ld_buffer, double *p_st_buffer);

void check_results (long long *p_n, double *p_scalar, double *p_ld_buffer, double *p_st_buffer)
{
  long long n   = *p_n;
  double scalar = *p_scalar;
  long long j, n_block;

  for (j=0; j<n; j++) {
    //printf ("[%lld]: st = %lf; ld = %lf\n", j, p_st_buffer[j], p_ld_buffer[j]); fflush(0);
    if (p_st_buffer[j] != scalar*p_ld_buffer[j]) {
      printf ("ERROR: validation failed at index-%lld. Expected = %lf, Observed = %lf\n", j, scalar*p_ld_buffer[j], p_st_buffer[j]);
      exit(1);
    }
  }
}

double mysecond()
{
	struct timeval mytime;

	gettimeofday( &mytime, (struct timezone *)0);

	return (double)(mytime.tv_sec) + (double)(mytime.tv_usec) * 1.e-6;
}

int main (int argc, char **argv)
{
  double *p_a = NULL, *p_buffer = NULL, *p_elapsed_time = NULL;
  char *p_align_str = NULL;
  double t_start, scalar;
  long long n, distance, j;
  size_t alignment;
  int i, k;
  int offset_start, offset_end, offset_step;
  int enable_ld_offset, enable_st_offset;

  if (argc != 7) {
    printf ("USAGE: %s\n"
            "\t<N>\n"
            "\t<alignment>\n"
            "\t<ld_offset>\n"
            "\t<st_offset>\n"
            "\t<offset_step>\n"
            "\t<distance_between_ld/st_buffers>\n"
            ,argv[0]);
    exit(1);
  }

  n                = atoll(argv[1]);
  p_align_str      = argv[2];
  enable_ld_offset = atoi(argv[3]);
  enable_st_offset = atoi(argv[4]);
  offset_step      = atoi(argv[5]);
  distance         = atoll(argv[6]);
  offset_start     = 0;
  offset_end       = MAX_OFFSET;
  scalar           = 2.;

  if (strcmp(p_align_str, "2M") == 0) {
    alignment = ALIGN_2M;
  } else if (strcmp(p_align_str, "4K") == 0) {
    alignment = ALIGN_4K;
  } else {
    alignment = atoi(p_align_str);
  }

  if (n > distance) {
    printf ("ERROR: distance (%lld) should be greater than or equal to n (%lld)\n", distance, n);
    exit(1);
  }

  long long size = distance + n + 2*MAX_OFFSET + ALIGN_4K/sizeof(double);

  double num_bytes_gb = (2. * n * sizeof(double))/1.e9;

  printf ("Number of elements per array   = %lld\n", n);
  printf ("Base alignment of buffer       = %s\n", p_align_str);
  printf ("Enable load buffer offset      = %d\n", enable_ld_offset);
  printf ("Enable store buffer offset     = %d\n", enable_st_offset);
  printf ("Step size of offset            = %d\n", offset_step);
  printf ("Distance b/w load,store buffer = %lld\n", distance);

  printf ("Number of bytes per array      = %.2f (%.2f MiB) \n", (1.* n * sizeof(double)), (1.*n*sizeof(double)/(1024.*1024.)));
  printf ("Total bytes processed          = %.2f GB\n", num_bytes_gb);
  printf ("\n");

  posix_memalign((void**)&p_a, alignment, (size*sizeof(double)));

  if (p_a == NULL) {
    printf ("ERROR: Memory allocation failed!\n");
    exit(1);
  }

  if ((alignment == ALIGN_4K) && ((unsigned long long)p_a & (ALIGN_2M-1) == 0)) {
      p_a += ALIGN_4K/sizeof(double);
  }
  p_buffer = p_a;

  posix_memalign((void**)&p_elapsed_time, 64, NTRIALS*sizeof(double));

  for (k=offset_start; k<=offset_end; k+=offset_step) {
    for (j=0; j<size; j++) {
      p_buffer[j] = k;
    }

    double best_time = FLT_MAX;
    double total_time = 0.;
    double *p_ld_buffer = ((enable_ld_offset) ? p_buffer+k : p_buffer);
    double *p_st_buffer = ((enable_st_offset) ? p_buffer+distance+k : p_buffer+distance);

    for (i=0; i<NTRIALS; i++) {
      t_start = TIMER();
      scale(&n, &scalar, p_ld_buffer, p_st_buffer);
      p_elapsed_time[i] = TIMER() - t_start;

      if (best_time > p_elapsed_time[i]) {
        best_time = p_elapsed_time[i];
      }
      if (i == 0) {
        printf ("Load Buffer Address  = %p\n", (void *)p_ld_buffer);
        printf ("Store Buffer Address = %p\n", (void *)p_st_buffer);

        check_results(&n, &scalar, p_ld_buffer, p_st_buffer);
      } else {
        total_time += p_elapsed_time[i];
      }
    }

    printf ("PERF (GB/s): N = %lld, alignment = %s, load_offset = %d, store_offset = %d, distance = %lld, Avg. BW = %.2f, Best BW = %.2f\n",
            n, p_align_str, ((enable_ld_offset) ? k : 0), ((enable_st_offset) ? k : 0), distance, num_bytes_gb/(total_time/(NTRIALS-1)), num_bytes_gb/best_time);
    fflush(0);
  }

  free(p_buffer);
  free(p_elapsed_time);

  return 0;
}
