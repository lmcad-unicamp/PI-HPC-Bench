cd ..
root=${PWD}

#tar -xzf build_scripts/tars/fftw-2.1.5.tar.gz
#tar -xf build_scripts/tars/hdf5-1.12.0.tar
#tar -xf build_scripts/tars/hdf5-1.10.5.tar.gz
#tar -xvf build_scripts/tars/gsl-latest.tar.gz
tar -xvf build_scripts/tars/hdf5-1.6.0.tar.gz

#cd fftw-2.1.5
#./configure --enable-float --enable-type-prefix --enable-static --prefix=${root}/fftw
#make -j 2
#sudo make install
#cd ..

#cd gsl-2.6
#./configure
#make -j 2
#sudo make install

#cd hdf5-1.12.0.tar
cd hdf5-1.6.0
./configure
make -j 2
sudo make install
cd ..

cd Gadget2
make -j 2
