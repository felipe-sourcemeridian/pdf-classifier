#!/bin/sh
wget https://s3.amazonaws.com/classifier-process/data/classifier.tar.gz
tar -xvzf classifier.tar.gz
./install_classifier.sh
rm classifier.tar.gz
rm install_classifier.sh