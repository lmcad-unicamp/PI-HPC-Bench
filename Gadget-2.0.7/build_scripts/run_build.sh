cd ..
root=${PWD}

tar -xzf build_scripts/tars/fftw-2.1.5.tar.gz
tar -xf build_scripts/tars/hdf5-1.12.0.tar
tar -xvf build_scripts/tars/gsl-latest.tar.gz

cd fftw-2.1.5
./configure --enable-float --enable-type-prefix --enable-static --prefix=${root}/fftw
make -j 2
sudo make install
cd ..

cd gsl-2.6
./configure
make -j 2

cd hdf5-1.12.0.tar
./configure
make -j 2
cd ..

cd Gadget2
make -j 2
