DATASET=exp-results
rm -rf $DATASET
mkdir $DATASET

sudo perf stat -o $DATASET/perf-app-graph.out ./graph500_reference_bfs 20
/usr/bin/time -o $DATASET/time-app-graph.out -v ./graph500_reference_bfs 20
ltrace -o $DATASET/mpi-app-graph.out ./graph500_reference_bfs 20

cat $DATASET/mpi-app-graph.out | grep "MPI" > $DATASET/mpi2-app-graph.out
rm $DATASET/mpi-app-graph.out
mv $DATASET/mpi2-app-graph.out $DATASET/mpi-app-graph.out
