#!/bin/bash
# clone ECP
git clone https://github.com/thaisacs/PI-HPC-Bench.git --recursive
cd PI-HPC-Bench
git submodule foreach git checkout master
git submodule foreach git pull origin master
cd ~
# build ECP
cd PI-HPC-Bench/utils
make
cd ../Gadget-2.0.7/build_scripts
./run_build.sh
cd ~
# send to all nodes
gcloud auth login
ssh-keygen -t rsa
cd .ssh
cat id_rsa.pub >> authorized_keys
touch ~/.ssh/config && echo 'Host *' >> ~/.ssh/config
touch ~/.ssh/config && echo '    StrictHostKeyChecking no' >> ~/.ssh/config
for i in $(seq ${1} ${2})
do
  echo "#############intance-$i##############"
  gcloud compute copy-files ~/.ssh/authorized_keys instance-$i:~/.ssh/ --zone=us-central1-a
  gcloud compute copy-files ~/PI-HPC-Bench/Gadget-2.0.7 instance-$i:~/ --zone=us-central1-a
  ssh instance-$i "mkdir PI-HPC-Bench && mv Gadget-2.0.7 PI-HPC-Bench"
  ssh instance-$i "cd /home/$USER/PI-HPC-Bench/Gadget-2.0.7/fftw-2.1.5 && sudo make install"
  ssh instance-$i "cd /home/$USER/PI-HPC-Bench/Gadget-2.0.7/hdf5-1.6.0 && sudo make install"
  ssh instance-$i "PATH=$PATH:/home/$USER/PI-HPC-Bench/Gadget-2.0.7/fftw-2.1.5/mpi && PATH=$PATH:/home/$USER/PI-HPC-Bench/Gadget-2.0.7/hdf5-1.6.0"
  ssh instance-$i "echo 'foi instance-$i'"
  echo "#####################################"
done
