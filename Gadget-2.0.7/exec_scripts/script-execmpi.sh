DATASET=exp-results
rm -rf $DATASET
mkdir $DATASET

cd Gadget2

perf stat -o ../$DATASET/perf-app-gadget.out ./Gadget2 parameterfiles/gassphere.param
/usr/bin/time -o ../$DATASET/time-app-gadget.out -v ./Gadget2 parameterfiles/gassphere.param
ltrace -o ../$DATASET/mpi-app-gadget.out ./Gadget2 parameterfiles/gassphere.param

cd ..
cd $DATASET
cat mpi-app-gadget.out | grep "MPI" > mpi2-app-gadget.out
rm mpi-app-gadget.out
mv mpi2-app-gadget.out mpi-app-gadget.out
