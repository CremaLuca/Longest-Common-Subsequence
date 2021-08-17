#!/bin/bash

#SBATCH --job-name lcs_o3_n8
#SBATCH --error lcs_err_o3_n8_%j.err
#SBATCH --output lcs_out_o3_n8_%j.log
#SBATCH --partition allgroups
#SBATCH --ntasks 8
#SBATCH --mem 2G
#SBATCH --time 10:00

spack load intel-parallel-studio@professional.2019.4

mpirun -np 8 ./lcs_o3.o ../input_small.txt
mpirun -np 8 ./lcs_o3.o ../input_small.txt
mpirun -np 8 ./lcs_o3.o ../input_small.txt
mpirun -np 8 ./lcs_o3.o ../input_small.txt

mpirun -np 8 ./lcs_o3.o ../input_medium.txt
mpirun -np 8 ./lcs_o3.o ../input_medium.txt
mpirun -np 8 ./lcs_o3.o ../input_medium.txt
mpirun -np 8 ./lcs_o3.o ../input_medium.txt

mpirun -np 8 ./lcs_o3.o ../input_large.txt
mpirun -np 8 ./lcs_o3.o ../input_large.txt
mpirun -np 8 ./lcs_o3.o ../input_large.txt
mpirun -np 8 ./lcs_o3.o ../input_large.txt
