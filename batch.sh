#!/bin/bash -x
#SBATCH --gres=gpu:1
#SBATCH --time=00:06:00
#SBATCH --partition=el8
#SBATCH --ntasks=64
#SBATCH --nodes=8

module load spectrum-mpi cuda/11.2
###########################################################################################
#
# Launch N tasks per compute node allocated. Per below this launches 6 MPI rank per compute
# taskset insures that hyperthreaded cores are skipped.
###########################################################################################
#
#taskset -c 0-159:4 mpirun -N 6 /gpfs/u/home/PCPC/PCPCdtjn/barn/hw4/reduce-exe
sbatch -N ${1:-1} --partition=el8 --gres=gpu:6 -t 30 -o output.out ./run.sh ${2:- } ${3:- } ${4:- } ${5:- }