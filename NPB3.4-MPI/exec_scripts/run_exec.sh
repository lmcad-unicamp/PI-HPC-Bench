#!/bin/bash
DATASET="$PWD/$1"
NAME=$(date +"%m-%d-%y-%T")
mkdir $DATASET
cd ../bin
mpirun -n $2 --hostfile hostfile ./bt.C.x > $DATASET/$NAME-BT-C.out 2> $DATASET/$NAME-BT-C.err
mpirun -n $2 --hostfile hostfile ./bt.D.x > $DATASET/$NAME-BT-D.out 2> $DATASET/$NAME-BT-D.err
mpirun -n $2 --hostfile hostfile ./bt.E.x > $DATASET/$NAME-BT-E.out 2> $DATASET/$NAME-BT-E.err
mpirun -n $2 --hostfile hostfile ./cg.C.x > $DATASET/$NAME-CG-C.out 2> $DATASET/$NAME-CG-C.err
mpirun -n $2 --hostfile hostfile ./cg.D.x > $DATASET/$NAME-CG-D.out 2> $DATASET/$NAME-CG-D.err
mpirun -n $2 --hostfile hostfile ./cg.E.x > $DATASET/$NAME-CG-E.out 2> $DATASET/$NAME-CG-E.err
mpirun -n $2 --hostfile hostfile ./is.C.x > $DATASET/$NAME-IS-C.out 2> $DATASET/$NAME-IS-C.err
mpirun -n $2 --hostfile hostfile ./is.D.x > $DATASET/$NAME-IS-D.out 2> $DATASET/$NAME-IS-D.err
mpirun -n $2 --hostfile hostfile ./is.E.x > $DATASET/$NAME-IS-E.out 2> $DATASET/$NAME-IS-E.err
mpirun -n $2 --hostfile hostfile ./mg.C.x > $DATASET/$NAME-MG-C.out 2> $DATASET/$NAME-MG-C.err
mpirun -n $2 --hostfile hostfile ./mg.D.x > $DATASET/$NAME-MG-D.out 2> $DATASET/$NAME-MG-D.err
mpirun -n $2 --hostfile hostfile ./mg.E.x > $DATASET/$NAME-MG-E.out 2> $DATASET/$NAME-MG-E.err
mpirun -n $2 --hostfile hostfile ./ft.C.x > $DATASET/$NAME-FT-C.out 2> $DATASET/$NAME-FT-C.err
mpirun -n $2 --hostfile hostfile ./ft.D.x > $DATASET/$NAME-FT-D.out 2> $DATASET/$NAME-FT-D.err
mpirun -n $2 --hostfile hostfile ./ft.E.x > $DATASET/$NAME-FT-E.out 2> $DATASET/$NAME-FT-E.err
mpirun -n $2 --hostfile hostfile ./sp.C.x > $DATASET/$NAME-SP-C.out 2> $DATASET/$NAME-SP-C.err
mpirun -n $2 --hostfile hostfile ./sp.D.x > $DATASET/$NAME-SP-D.out 2> $DATASET/$NAME-SP-D.err
mpirun -n $2 --hostfile hostfile ./sp.E.x > $DATASET/$NAME-SP-E.out 2> $DATASET/$NAME-SP-E.err
mpirun -n $2 --hostfile hostfile ./lu.C.x > $DATASET/$NAME-LU-C.out 2> $DATASET/$NAME-LU-C.err
mpirun -n $2 --hostfile hostfile ./lu.D.x > $DATASET/$NAME-LU-D.out 2> $DATASET/$NAME-LU-D.err
mpirun -n $2 --hostfile hostfile ./lu.E.x > $DATASET/$NAME-LU-E.out 2> $DATASET/$NAME-LU-E.err
mpirun -n $2 --hostfile hostfile ./ep.C.x > $DATASET/$NAME-EP-C.out 2> $DATASET/$NAME-EP-C.err
mpirun -n $2 --hostfile hostfile ./ep.D.x > $DATASET/$NAME-EP-D.out 2> $DATASET/$NAME-EP-D.err
mpirun -n $2 --hostfile hostfile ./ep.E.x > $DATASET/$NAME-EP-E.out 2> $DATASET/$NAME-EP-E.err
