#!/bin/bash

#SBATCH --job-name lcs_o1_n4
#SBATCH --error lcs_err_o1_n4_%j.err
#SBATCH --output lcs_out_o1_n4_%j.log
#SBATCH --partition allgroups
#SBATCH -c 1
#SBATCH --ntasks 4
#SBATCH --mem 32G
#SBATCH --time 30:00

spack load intel-parallel-studio@professional.2019.4

mpirun -np 4 ./lcs_o1.o ../input_small.txt
mpirun -np 4 ./lcs_o1.o ../input_small.txt
mpirun -np 4 ./lcs_o1.o ../input_small.txt
mpirun -np 4 ./lcs_o1.o ../input_small.txt

mpirun -np 4 ./lcs_o1.o ../input_medium.txt
mpirun -np 4 ./lcs_o1.o ../input_medium.txt
mpirun -np 4 ./lcs_o1.o ../input_medium.txt
mpirun -np 4 ./lcs_o1.o ../input_medium.txt

mpirun -np 4 ./lcs_o1.o ../input_large.txt
mpirun -np 4 ./lcs_o1.o ../input_large.txt
mpirun -np 4 ./lcs_o1.o ../input_large.txt
mpirun -np 4 ./lcs_o1.o ../input_large.txt

mpirun -np 4 ./lcs_o1.o ../input_large2.txt
mpirun -np 4 ./lcs_o1.o ../input_large2.txt

mpirun -np 4 ./lcs_o1.o ../input_large3.txt
