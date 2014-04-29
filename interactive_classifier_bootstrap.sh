#!/bin/sh
sudo apt-get update
sudo apt-get -q -y install  libglib2.0 unzip
sudo apt-get -q -y install libicu48
wget https://s3.amazonaws.com/classifier-process/data/interactive_classifier.tar.gz
tar -xvzf interactive_classifier.tar.gz
sudo dpkg -i classifier_0.1_amd64.deb
sudo apt-get -q -y -f install

set -e
sudo ./install_classifier.sh $1
rm -f interactive_classifier.tar.gz
rm -f install_classifier.sh
rm -f classifier.dbg
