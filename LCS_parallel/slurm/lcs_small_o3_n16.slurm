#!/bin/bash

#SBATCH --job-name lcs_o3_n16
#SBATCH --error lcs_o3_n16_%j.txt
#SBATCH --output lcs_out_o3_n16_%j.txt
#SBATCH --partition allgroups
#SBATCH --mail-user luca.crema.1@studenti.unipd.it
#SBATCH --mail-type ALL
#SBATCH --ntasks 16
#SBATCH --mem 2G
#SBATCH --time 0:30

spack load intel-parallel-studio@professional.2019.4
srun lcs_o3.o input_small.txt out_small_o3_n16.txt
