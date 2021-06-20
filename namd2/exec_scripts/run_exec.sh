#!/bin/bash
HOSTFILE=$PWD/hostfile
DATASET=$PWD/$1
NAME=$(date +"%m-%d-%y-%T")
mkdir $DATASET
cd ..
namd_a_app="./Linux-x86_64-g++/namd2 input/apoa1/apoa1.namd"
namd_b_app="./Linux-x86_64-g++/namd2 input/f1atpase/f1atpase.namd "
namd_c_app="./Linux-x86_64-g++/namd2 input/stmv/stmv.namd"
# APOA1
mpirun -n $2 --hostfile $HOSTFILE $namd_a_app > $DATASET/$NAME-namd-A.out 2> $DATASET/$NAME-namd-A.err
mpirun -n $2 --hostfile $HOSTFILE $namd_b_app > $DATASET/$NAME-namd-B.out 2> $DATASET/$NAME-namd-B.err
mpirun -n $2 --hostfile $HOSTFILE $namd_c_app > $DATASET/$NAME-namd-C.out 2> $DATASET/$NAME-namd-C.err
