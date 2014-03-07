#!/bin/sh
DEBIAN_FRONTED=noninteractive apt-get -q -y install libicu48 libglib2.0 unzip
dpkg -i classifier_0.1_amd64.deb
wget https://s3.amazonaws.com/classifier-process/data/file_transformation.zip
unzip -e file_transformation.zip
mv *.data /usr/local/classifier_static_data/
mv *.aho /usr/local/classifier_static_data/
/etc/init.d/classifier start
cp /etc/init.d/classifier /usr/local/bin/
rm file_transformation.zip
rm classifier_0.1_amd64.deb
