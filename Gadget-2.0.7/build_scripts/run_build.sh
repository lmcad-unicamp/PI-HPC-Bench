cd ..
root=${PWD}
tar -xzf build_scripts/tars/fftw-2.1.5.tar.gz
tar -xvf build_scripts/tars/gsl-latest.tar.gz
tar -xvf build_scripts/tars/hdf5-1.6.0.tar.gz
cd fftw-2.1.5
./configure --enable-float --enable-type-prefix --enable-static --prefix=${root}/fftw
make
sudo make install
cd ..
cd gsl-2.6
./configure
make
sudo make install
cd hdf5-1.6.0
./configure
make
sudo make install
cd ..
cd Gadget2
make 
