#include <immintrin.h>
#include <stdio.h>

#undef  ENABLE_PROLOG

void scale (long long *p_n, double *p_scalar, double *p_ld_buffer, double *p_st_buffer)
{
  long long n   = *p_n;
  double scalar = *p_scalar;
  long long j, n_block, n_tail;
  int alignment, p=0;

#if defined ENABLE_PROLOG
#if defined (__AVX512F__)
  alignment = 64;
#elif defined (__AVX2__) || defined (__AVX__)
  alignment = 32;
#else
  alignment = 16;
#endif

  int prolog_bytes = (p_st_buffer & (alignment-1));
  if (prolog_bytes) {
    p = (alignment - prolog_bytes)/8;
    for (j=0; j<p; j++) {
      p_st_buffer[j] = scalar*p_ld_buffer[j];
    }
    p_st_buffer += p;
    p_ld_buffer += p;
  }
#endif

#if defined (__AVX512F__)
  __m512d zmm_a, zmm_b, zmm_s;
  zmm_s = _mm512_broadcastsd_pd(_mm_load_sd(&scalar));
  n_block = (n/8)*8;
  for (j=p; j<n_block; j+=8) {
    zmm_a = _mm512_loadu_pd(p_ld_buffer);
    zmm_a = _mm512_mul_pd(zmm_s,zmm_a);
#if defined (USE_NT_STORES)
    _mm512_stream_pd(p_st_buffer, zmm_a);
#elif defined (USE_RFO_STORES)
    _mm512_storeu_pd(p_st_buffer, zmm_a);
#endif
    p_ld_buffer += 8;
    p_st_buffer += 8;
  }
#elif defined (__AVX2__) || defined (__AVX__)
  __m256d ymm_a, ymm_b, ymm_s;
  ymm_s = _mm256_broadcast_sd(&scalar);
  n_block = (n/4)*4;
  for (j=p; j<n_block; j+=4) {
    ymm_a = _mm256_loadu_pd(p_ld_buffer);
    ymm_a = _mm256_mul_pd(ymm_s,ymm_a);
#if defined (USE_NT_STORES)
    _mm256_stream_pd(p_st_buffer, ymm_a);
#elif defined (USE_RFO_STORES)
    _mm256_storeu_pd(p_st_buffer, ymm_a);
#endif
    p_ld_buffer += 4;
    p_st_buffer += 4;
  }
#else
  __m128d xmm_a, xmm_b, xmm_s;
  xmm_s = _mm_loaddup_pd(&scalar);
  n_block = (n/2)*2;
  for (j=p; j<n_block; j+=2) {
    xmm_a = _mm_loadu_pd(p_ld_buffer);
    xmm_a = _mm_mul_pd(xmm_s,xmm_a);
#if defined (USE_NT_STORES)
    _mm_stream_pd(p_st_buffer, xmm_a);
#elif defined (USE_RFO_STORES)
    _mm_storeu_pd(p_st_buffer, xmm_a);
#endif
    p_ld_buffer += 2;
    p_st_buffer += 2;
  }
#endif

  n_tail = n - n_block;
  while (n_tail) {
    *p_st_buffer = scalar**p_ld_buffer;
    p_st_buffer++;
    p_ld_buffer++;
    n_tail--;
  }
}

