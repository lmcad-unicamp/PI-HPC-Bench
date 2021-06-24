cd ..
root=${PWD}
tar -xzf build_scripts/tars/fftw-2.1.5.tar.gz
tar -xvf build_scripts/tars/gsl-latest.tar.gz
tar -xvf build_scripts/tars/hdf5-1.6.0.tar.gz
cd fftw-2.1.5
./configure --enable-mpi --enable-type-prefix --enable-float
make
sudo make install
cd ..
cd gsl-2.6
./configure
make
sudo make install
cd hdf5-1.6.0.tar.gz
./configure
make
sudo make install
cd ..
cd Gadget2
make
PATH=$PATH:/home/$USER/PI-HPC-Bench/Gadget-2.0.7/fftw-2.1.5/mpi
PATH=$PATH:/home/$USER/PI-HPC-Bench/Gadget-2.0.7/hdf5-1.6.0
