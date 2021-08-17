#!/bin/bash

#SBATCH --job-name lcs_o3_n16
#SBATCH --error lcs_err_o3_n16_%j.err
#SBATCH --output lcs_out_o3_n16_%j.log
#SBATCH --partition allgroups
#SBATCH --ntasks 16
#SBATCH --mem 2G
#SBATCH --time 10:00

spack load intel-parallel-studio@professional.2019.4

mpirun -np 16 ./lcs_o3.o ../input_small.txt
mpirun -np 16 ./lcs_o3.o ../input_small.txt
mpirun -np 16 ./lcs_o3.o ../input_small.txt
mpirun -np 16 ./lcs_o3.o ../input_small.txt

mpirun -np 16 ./lcs_o3.o ../input_medium.txt
mpirun -np 16 ./lcs_o3.o ../input_medium.txt
mpirun -np 16 ./lcs_o3.o ../input_medium.txt
mpirun -np 16 ./lcs_o3.o ../input_medium.txt

mpirun -np 16 ./lcs_o3.o ../input_large.txt
mpirun -np 16 ./lcs_o3.o ../input_large.txt
mpirun -np 16 ./lcs_o3.o ../input_large.txt
mpirun -np 16 ./lcs_o3.o ../input_large.txt
