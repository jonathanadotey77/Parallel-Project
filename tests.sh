#!/bin/bash -x

module load spectrum-mpi cuda/11.2

sbatch --gres=gpu:6 --time=00:06:00 --partition=el8 --ntasks=32 --nodes=1 -o output1.out ./run.sh bin100k.bin -v

sbatch --gres=gpu:6 --time=00:04:00 --partition=el8 --ntasks=64 --nodes=2 -o output2.out ./run.sh bin100k.bin -v

sbatch --gres=gpu:6 --time=00:02:20 --partition=el8 --ntasks=128 --nodes=4 -o output4.out ./run.sh bin100k.bin -v 

sbatch --gres=gpu:6 --time=00:01:20 --partition=el8 --ntasks=256 --nodes=8 -o output8.out ./run.sh bin100k.bin -v 

sbatch --gres=gpu:6 --time=00:01:00 --partition=el8 --ntasks=512 --nodes=16 -o output16.out ./run.sh bin100k.bin -v 