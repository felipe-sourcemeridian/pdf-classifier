#!/bin/sh
REAL_USER=$(whoami)
CLASSIFIER_DATA_DIR=/usr/local/classifier_static_data/
sudo apt-get install libicu-dev
sudo apt-get install binutils
sudo apt-get install libicu-dev
sudo apt-get install libglib2.0-dev
sudo apt-get install zip
sudo apt-get install gdb
wget https://s3.amazonaws.com/classifier-process/data/file_transformation.zip
sudo mkdir $CLASSIFIER_DATA_DIR
sudo chown $REAL_USER -R $CLASSIFIER_DATA_DIR
unzip -e file_transformation.zip
mv *.data $CLASSIFIER_DATA_DIR
mv *.aho $CLASSIFIER_DATA_DIR
rm -f node_tax_issue.txt
sudo cp package/classifier/etc/classifier.conf /etc/classifier.conf
sudo chown $REAL_USER /etc/classifier.conf



