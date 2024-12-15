# Load Store Bench

Microbenchmark to measure load/store ops and identify performance anomalies such as 4K aliasing 

![image](https://github.com/user-attachments/assets/0da6f21d-c807-4081-b17f-1abed89ce062)

Load-Store micro-benchmark to measure performance with varying alignment, offset values between load/store buffers, written in Compiler Intrinsics
 - SSE, AVX, AVX512. All loads/stores are on aligned (16B|32B|64B) addresses.
 - Store can be NT (vmovntpd) or RFO (vmovupd)

Parameters
- Dynamic memory management, alignment, relative offsets, distance between load/store buffers
- N: Number of elements (FP64) to be copied from Load to Store Buffer
- D: Load and Store Buffers are separated by d elements (d>=N)
- Base Alignment: Any power of 2 alignment, covers page alignment cases of 2MB or 4KB
- Load/Store Offset: Number of elements from buffer: Primary parameter to tweak/adjust the lower address bits of load/store buffer addresses.
  
