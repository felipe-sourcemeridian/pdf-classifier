#!/bin/sh
set -e
sudo wget $1 
sudo unzip -e file_transformation.zip
sudo mv -f *.data /usr/local/classifier_static_data/
sudo mv -f *.aho /usr/local/classifier_static_data/
sudo mkdir -p /root/cores
sudo chmod a+xrwx /root/cores
sudo /etc/init.d/classifier start
sudo mv -f classifier_error_recovery.py /usr/local/bin/classifier_error_recovery
sudo cp /etc/init.d/classifier /usr/local/bin/
sudo rm -f file_transformation.zip
sudo rm -f classifier_0.1_amd64.deb
sudo nohup classifier_error_recovery > error.log 2>&1 &
sudo tail /var/log/syslog
sudo ls -lah /usr/local/classifier_static_data/
