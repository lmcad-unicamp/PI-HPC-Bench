gcloud auth login
ssh-keygen -t rsa
cd .ssh
cat id_rsa.pub >> authorized_keys
gcloud compute instance-1:~/.ssh/  ~/.ssh/authorized_keys --zone=us-central1-a
#gcloud compute copy-files id_rsa.pub
#gcloud compute ssh instance_1
