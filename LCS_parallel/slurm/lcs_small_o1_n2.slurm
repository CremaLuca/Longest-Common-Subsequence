#!/bin/bash

#SBATCH --job-name lcs_o1_n2
#SBATCH --error lcs_o1_n2_%j.txt
#SBATCH --output lcs_out_o1_n2_%j.txt
#SBATCH --partition allgroups
#SBATCH --mail-user luca.crema.1@studenti.unipd.it
#SBATCH --mail-type ALL
#SBATCH --ntasks 2
#SBATCH --mem 2G
#SBATCH --time 0:30

spack load intel-parallel-studio@professional.2019.4
srun lcs_o1.o input_small.txt out_small_o1_n2.txt
