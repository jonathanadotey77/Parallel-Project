#!/bin/bash -x
#SBATCH --gres=gpu:6
#SBATCH --time=00:02:00
#SBATCH --partition=el8
#SBATCH --ntasks=160
#SBATCH --nodes=8

module load spectrum-mpi cuda/11.2
###########################################################################################
#
# Launch N tasks per compute node allocated. Per below this launches 6 MPI rank per compute
# taskset insures that hyperthreaded cores are skipped.
###########################################################################################
#
taskset -c 0-159:6 mpirun -N 32 /gpfs/u/home/PCPC/PCPCdtjn/barn/git/Parallel-Project/simulate ${1:- } ${2:- } ${3:- } ${4:- }