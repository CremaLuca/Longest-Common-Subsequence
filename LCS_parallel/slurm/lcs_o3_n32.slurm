#!/bin/bash

#SBATCH --job-name lcs_o3_n32
#SBATCH --error lcs_err_o3_n32_%j.err
#SBATCH --output lcs_out_o3_n32_%j.log
#SBATCH --partition allgroups
#SBATCH --ntasks 32
#SBATCH --mem 4G
#SBATCH --time 25:00

spack load intel-parallel-studio@professional.2019.4

mpirun -np 32 ./lcs_o3.o ../input_small.txt
mpirun -np 32 ./lcs_o3.o ../input_small.txt
mpirun -np 32 ./lcs_o3.o ../input_small.txt
mpirun -np 32 ./lcs_o3.o ../input_small.txt

mpirun -np 32 ./lcs_o3.o ../input_medium.txt
mpirun -np 32 ./lcs_o3.o ../input_medium.txt
mpirun -np 32 ./lcs_o3.o ../input_medium.txt
mpirun -np 32 ./lcs_o3.o ../input_medium.txt

mpirun -np 32 ./lcs_o3.o ../input_large.txt
mpirun -np 32 ./lcs_o3.o ../input_large.txt
mpirun -np 32 ./lcs_o3.o ../input_large.txt
mpirun -np 32 ./lcs_o3.o ../input_large.txt

