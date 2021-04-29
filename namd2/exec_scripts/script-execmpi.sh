DATASET=exp-results
rm -rf $DATASET
mkdir $DATASET

sudo perf stat -o $DATASET/perf-app-namd.out ./Linux-x86_64-g++/namd2 input/apoa1/apoa1.namd
/usr/bin/time -o $DATASET/time-app-namd.out -v ./Linux-x86_64-g++/namd2 input/apoa1/apoa1.namd
ltrace -o $DATASET/mpi-app-namd.out ./Linux-x86_64-g++/namd2 input/apoa1/apoa1.namd

cd $DATASET
cat mpi-app-namd.out | grep "MPI" > mpi2-app-namd.out
rm mpi-app-namd.out
mv mpi2-app-namd.out mpi-app-namd.out

#mpirun -n 2 ./lmp -var t 300 -echo screen -in lj/in.lj
