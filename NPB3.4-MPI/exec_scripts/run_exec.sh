#!/bin/bash
DATASET=$PWD"/exp-dataset"
NAME=$(date +"%m-%d-%y-%T")
mkdir -f $DATASET
cd ../bin
./bt.C.x > $DATASET/$NAME-BT-C.out
./bt.D.x > $DATASET/$NAME-BT-D.out
./bt.E.x > $DATASET/$NAME-BT-E.out
./cg.C.x > $DATASET/$NAME-BT-C.out
./cg.D.x > $DATASET/$NAME-BT-D.out
./cg.E.x > $DATASET/$NAME-BT-E.out
./ep.C.x > $DATASET/$NAME-BT-C.out
./ep.D.x > $DATASET/$NAME-BT-D.out
./ep.E.x > $DATASET/$NAME-BT-E.out
./is.C.x > $DATASET/$NAME-BT-C.out
./is.D.x > $DATASET/$NAME-BT-D.out
./is.E.x > $DATASET/$NAME-BT-E.out
./mg.C.x > $DATASET/$NAME-BT-C.out
./mg.D.x > $DATASET/$NAME-BT-D.out
./mg.E.x > $DATASET/$NAME-BT-E.out
./ft.C.x > $DATASET/$NAME-BT-C.out
./ft.D.x > $DATASET/$NAME-BT-D.out
./ft.E.x > $DATASET/$NAME-BT-E.out
./bt.C.x > $DATASET/$NAME-BT-C.out
./bt.D.x > $DATASET/$NAME-BT-D.out
./bt.E.x > $DATASET/$NAME-BT-E.out
./sp.C.x > $DATASET/$NAME-BT-C.out
./sp.D.x > $DATASET/$NAME-BT-D.out
./sp.E.x > $DATASET/$NAME-BT-E.out
./lu.C.x > $DATASET/$NAME-BT-C.out
./lu.D.x > $DATASET/$NAME-BT-D.out
./lu.E.x > $DATASET/$NAME-BT-E.out
