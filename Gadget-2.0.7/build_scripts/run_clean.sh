cd ..
sudo rm -rf fftw
rm -fr fftw-2.1.5
rm -fr hdf5-1.12.0
rm -fr gsl-2.6

cd Gadget2
rm -r galaxy
rm -r gassphere
rm parameterfiles/*.param-usedvalues
make clean
