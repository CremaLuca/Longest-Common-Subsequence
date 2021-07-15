#!/bin/bash

#SBATCH --job-name lcs
#SBATCH --error lcs_%j.txt
#SBATCH --output lcs_out_%j.txt
#SBATCH --partition allgroups
#SBATCH --ntasks 2
#SBATCH -c 1
#SBATCH --mem 1G
#SBATCH --time 0:30


spack load intel-parallel-studio@professional.2019.4
mpirun -np 2 ./lcs.o