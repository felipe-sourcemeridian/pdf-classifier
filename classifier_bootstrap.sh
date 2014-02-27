#!/bin/sh
wget https://s3.amazonaws.com/classifier/classifier.tar.gz
tar -xvzf classifier.tar.gz
cd classifier && ./install_classifier.sh
