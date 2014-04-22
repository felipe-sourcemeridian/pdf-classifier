#!/bin/sh
wget https://s3.amazonaws.com/classifier-process/data/interactive_classifier.tar.gz
tar -xvzf classifier.tar.gz
sudo ./install_classifier.sh $1
rm classifier.tar.gz
rm install_classifier.sh
rm classifier.dbg
