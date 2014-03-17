#!/bin/sh
sudo apt-get update
sudo DEBIAN_FRONTED=noninteractive apt-get -q -y install  libglib2.0 unzip
sudo apt-get -q -y install libicu48
sudo DEBIAN_FRONTED=noninteractive apt-get -q -y -f install
sudo dpkg -i classifier_0.1_amd64.deb
sudo wget https://s3.amazonaws.com/classifier-process/data/file_transformation.zip
sudo unzip -e file_transformation.zip
sudo mv -f *.data /usr/local/classifier_static_data/
sudo mv -f *.aho /usr/local/classifier_static_data/
sudo mkdir -p /root/cores
sudo chmod a+xrwx /root/cores
#sudo echo "/root/cores/cores.%e.%p.%h.%t" > /proc/sys/kernel/core_pattern
sudo /etc/init.d/classifier start
sudo mv -f classifier_error_recovery.py /usr/local/bin/classifier_error_recovery
sudo cp /etc/init.d/classifier /usr/local/bin/
sudo rm -f file_transformation.zip
sudo rm -f classifier_0.1_amd64.deb
sudo nohup classifier_error_recovery > error.log 2>&1 &
