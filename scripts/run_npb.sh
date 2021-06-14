#!/bin/bash
git config --global user.name "thais"
git config --global user.email "thaiscamachoo@gmail.com"
git clone https://github.com/thaisacs/PI-HPC-Bench
cd PI-HPC-Bench/utils
make
cd ../NPB3.4-MPI/build_scripts/
./run_build.sh
cd ~
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
