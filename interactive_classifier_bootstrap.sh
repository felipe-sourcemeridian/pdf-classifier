#!/bin/sh
wget https://s3.amazonaws.com/classifier-process/data/interactive_classifier.tar.gz
tar -xvzf interactive_classifier.tar.gz
sudo ./install_classifier.sh $1
rm -f interactive_classifier.tar.gz
rm -f install_classifier.sh
rm -f classifier.dbg
