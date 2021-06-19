#!/bin/bash
HOSTFILE=$PWD/hostfile
DATASET=$PWD/$1
NAME=$(date +"%m-%d-%y-%T")
mkdir $DATASET
cd ..
gadget_a_app="./Gadget2 parameterfiles/gassphere.param"
gadget_b_app="./Gadget2 parameterfiles/galaxy.param"
cd Gadget2
mkdir galaxy
mkdir gassphere
mpirun -n $2 --hostfile $HOSTFILE $gadget_b_app > $DATASET/$NAME-gadget-B.out 2> $DATASET/$NAME-gadget-B.err
mpirun -n $2 --hostfile $HOSTFILE $gadget_c_app > $DATASET/$NAME-gadget-C.out 2> $DATASET/$NAME-gadget-C.err
cd ..
