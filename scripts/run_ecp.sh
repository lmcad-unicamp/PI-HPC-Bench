#!/bin/bash
#build cmake
wget https://github.com/Kitware/CMake/releases/download/v3.20.3/cmake-3.20.3.tar.gz
tar -xvzf cmake-3.20.3.tar.gz
cd cmake-3.20.3
./configure
make
sudo make install
PATH=$PATH:~/cmake-3.20.3/bin/cmake
cd ~
#build json
git clone https://github.com/LLNL/json-cwx.git
cd json-cwx/json-cwx
sh autogen.sh
./configure
make
sudo make install
cd ~
# config github
git config --global user.name "thais"
git config --global user.email "thaiscamachoo@gmail.com"
# clone ECP
git clone https://github.com/thaisacs/PI-HPC-Bench.git --recursive
cd PI-HPC-Bench
git submodule foreach git checkout master
git submodule foreach git pull origin master
cd ECP-Proxy-Apps
git submodule foreach git checkout master
git submodule foreach git pull origin master
# build ECP
cd PI-HPC-Bench/utils
make
cd ../ECP-Proxy-Apps/build_scripts
./run_build.sh
./run_build_app.sh
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
  gcloud compute copy-files ~/.ssh/authorized_keys instance-$i:~/.ssh/ --zone=us-central1-a
  gcloud compute copy-files ~/PI-HPC-Bench instance-$i:~/ --zone=us-central1-a
done
