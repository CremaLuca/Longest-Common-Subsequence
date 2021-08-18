#!/bin/bash

#SBATCH --job-name lcs_seq
#SBATCH --error lcs_err_seq_%j.err
#SBATCH --output lcs_out_seq_%j.log
#SBATCH --partition allgroups
#SBATCH --ntasks 1
#SBATCH --mem 16G
#SBATCH --time 60:00

./main_o1.o ../input_small.txt
./main_o1.o ../input_medium.txt
./main_o1.o ../input_large.txt
./main_o1.o ../input_large2.txt

./main_o2.o ../input_small.txt
./main_o2.o ../input_medium.txt
./main_o2.o ../input_large.txt
./main_o2.o ../input_large2.txt

./main_o3.o ../input_small.txt
./main_o3.o ../input_medium.txt
./main_o3.o ../input_large.txt
./main_o3.o ../input_large2.txt

./main_o1.o ../input_large3.txt
./main_o2.o ../input_large3.txt
./main_o3.o ../input_large3.txt
