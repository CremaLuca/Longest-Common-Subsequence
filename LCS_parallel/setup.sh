#!/bin/bash

spack load intel-parallel-studio@professional.2019.4

mpiicpc utils.cpp main.cpp -O1 -o lcs_o1.o
mpiicpc utils.cpp main.cpp -O3 -o lcs_o3.o

find . -type f -print0 | xargs -0 dos2unix
