#!/bin/bash
set -v

FLAGS="-xCORE-AVX512 -qopt-zmm-usage=high"
# FLAGS="-march=core-avx2"
#FLAGS="-march=pentium4"

icx -qmkl -DUSE_NT_STORES -O3 ${FLAGS} -fno-alias -fno-builtin bench.c kernel.c -o s2l-nt.out
icx -qmkl -DUSE_RFO_STORES -O3 ${FLAGS} -fno-alias -fno-builtin bench.c kernel.c -o s2l-rfo.out
