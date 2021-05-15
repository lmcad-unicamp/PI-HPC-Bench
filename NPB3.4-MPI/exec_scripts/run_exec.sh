#!/bin/bash
DATASET="$PWD/$1"
NAME=$(date +"%m-%d-%y-%T")
mkdir $DATASET
cd ../bin
mpirun -n $2 ./bt.C.x > $DATASET/$NAME-BT-C.out 2> $DATASET/$NAME-BT-C.err
mpirun -n $2 ./bt.D.x > $DATASET/$NAME-BT-D.out 2> $DATASET/$NAME-BT-D.err
mpirun -n $2 ./bt.E.x > $DATASET/$NAME-BT-E.out 2> $DATASET/$NAME-BT-E.err
mpirun -n $2 ./cg.C.x > $DATASET/$NAME-CG-C.out 2> $DATASET/$NAME-CG-C.err
mpirun -n $2 ./cg.D.x > $DATASET/$NAME-CG-D.out 2> $DATASET/$NAME-CG-D.err
mpirun -n $2 ./cg.E.x > $DATASET/$NAME-CG-E.out 2> $DATASET/$NAME-CG-E.err
mpirun -n $2 ./is.C.x > $DATASET/$NAME-IS-C.out 2> $DATASET/$NAME-IS-C.err
mpirun -n $2 ./is.D.x > $DATASET/$NAME-IS-D.out 2> $DATASET/$NAME-IS-D.err
mpirun -n $2 ./is.E.x > $DATASET/$NAME-IS-E.out 2> $DATASET/$NAME-IS-E.err
mpirun -n $2 ./mg.C.x > $DATASET/$NAME-MG-C.out 2> $DATASET/$NAME-MG-C.err
mpirun -n $2 ./mg.D.x > $DATASET/$NAME-MG-D.out 2> $DATASET/$NAME-MG-D.err
mpirun -n $2 ./mg.E.x > $DATASET/$NAME-MG-E.out 2> $DATASET/$NAME-MG-E.err
mpirun -n $2 ./ft.C.x > $DATASET/$NAME-FT-C.out 2> $DATASET/$NAME-FT-C.err
mpirun -n $2 ./ft.D.x > $DATASET/$NAME-FT-D.out 2> $DATASET/$NAME-FT-D.err
mpirun -n $2 ./ft.E.x > $DATASET/$NAME-FT-E.out 2> $DATASET/$NAME-FT-E.err
mpirun -n $2 ./sp.C.x > $DATASET/$NAME-SP-C.out 2> $DATASET/$NAME-SP-C.err
mpirun -n $2 ./sp.D.x > $DATASET/$NAME-SP-D.out 2> $DATASET/$NAME-SP-D.err
mpirun -n $2 ./sp.E.x > $DATASET/$NAME-SP-E.out 2> $DATASET/$NAME-SP-E.err
mpirun -n $2 ./lu.C.x > $DATASET/$NAME-LU-C.out 2> $DATASET/$NAME-LU-C.err
mpirun -n $2 ./lu.D.x > $DATASET/$NAME-LU-D.out 2> $DATASET/$NAME-LU-D.err
mpirun -n $2 ./lu.E.x > $DATASET/$NAME-LU-E.out 2> $DATASET/$NAME-LU-E.err
mpirun -n $2 ./ep.C.x > $DATASET/$NAME-EP-C.out 2> $DATASET/$NAME-EP-C.err
mpirun -n $2 ./ep.D.x > $DATASET/$NAME-EP-D.out 2> $DATASET/$NAME-EP-D.err
mpirun -n $2 ./ep.E.x > $DATASET/$NAME-EP-E.out 2> $DATASET/$NAME-EP-E.err
