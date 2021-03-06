#!/bin/bash
cd ..
root=${PWD}
tar xvf build_scripts/tars/charm-6.10.2.tar.gz
tar xzf build_scripts/tars/fftw-2.1.5.tar.gz
tar xzf build_scripts/tars/tcl8.5.9-linux-x86_64.tar.gz
tar xzf build_scripts/tars/tcl8.5.9-linux-x86_64-threaded.tar.gz
mv tcl8.5.9-linux-x86_64 tcl
mv tcl8.5.9-linux-x86_64-threaded tcl-threaded
mv charm-v6.10.2 charm-6.10.2
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
export PATH=$PATH:${PWD}/charm-6.10.2
cd Linux-x86_64-g++
make
cd ../input
tar -xvf apoa1.tar
tar -xvf f1atpase.tar
