#!/bin/bash -x

module load spectrum-mpi cuda/11.2
###########################################################################################
#
# Launch N tasks per compute node allocated. Per below this launches 6 MPI rank per compute
# taskset insures that hyperthreaded cores are skipped.
###########################################################################################
#
taskset -c 0-159:6 mpirun -N 6 /gpfs/u/home/PCPC/PCPCdtjn/barn/project/sim