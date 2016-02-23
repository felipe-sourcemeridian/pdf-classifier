#!/bin/sh
sudo yum install -y libicu.x86_64 glib2.x86_64
wget https://s3.amazonaws.com/classifier-process/data/classifier.tar.gz
wget https://s3.amazonaws.com/classifier-process/data/file_transformation.zip
tar -xvzf  classifier.tar.gz
unzip -e file_transformation.zip
sudo mkdir /usr/local/classifier_static_data/
sudo mv *.data /usr/local/classifier_static_data/
sudo mv *.aho /usr/local/classifier_static_data/
sudo mv classifier/classifier /usr/local/bin
sudo mv classifier/classifier.conf /etc
sudo mv classifier/classifier_error_recovery.py /usr/local/bin
sudo /usr/local/bin/classifier /etc/classifier.conf
sudo nohup /usr/local/bin/classifier_error_recovery.py > error.log 2>&1  &
sudo rm -rf classifier
