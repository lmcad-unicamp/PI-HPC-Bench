#!/bin/bash

cd ..
root=${PWD}

tar xvf build_scripts/tars/charm-6.10.2.tar
tar xzf build_scripts/tars/fftw-2.1.5.tar.gz
tar xzf build_scripts/tars/tcl8.5.9-linux-x86_64.tar.gz
tar xzf build_scripts/tars/tcl8.5.9-linux-x86_64-threaded.tar.gz

mv tcl8.5.9-linux-x86_64 tcl
mv tcl8.5.9-linux-x86_64-threaded tcl-threaded

cd charm-6.10.2
env MPICXX=mpicxx ./build charm++ mpi-linux-x86_64 --with-production
cd mpi-linux-x86_64/tests/charm++/megatest
make pgm
mpiexec -n 2 ./pgm
cd ../../../../..

cd fftw-2.1.5
./configure --enable-float --enable-type-prefix --enable-static --prefix=${root}/fftw
make
sudo make install
cd ..

./config Linux-x86_64-g++ --charm-arch mpi-linux-x86_64
cd Linux-x86_64-g++
make -j 2
