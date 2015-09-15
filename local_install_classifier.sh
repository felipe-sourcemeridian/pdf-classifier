#!/bin/sh
echo "deb http://security.ubuntu.com/ubuntu precise-security main\n" >> /etc/apt/sources.list
apt-get update
apt-get -q -y install  libglib2.0 unzip
apt-get -q -y install libicu48
wget https://s3.amazonaws.com/classifier-process/data/interactive_classifier.tar.gz
tar -xvzf interactive_classifier.tar.gz
dpkg -i classifier_0.1_amd64.deb
apt-get -q -y -f install
set -e
wget https://s3.amazonaws.com/classifier-process/data/file_transformation.zip
unzip -e file_transformation.zip
mv -f *.data /usr/local/classifier_static_data/
mv -f *.aho /usr/local/classifier_static_data/
mkdir -p /root/cores
chmod a+xrwx /root/cores
/etc/init.d/classifier start
mv -f classifier_error_recovery.py /usr/local/bin/classifier_error_recovery
cp /etc/init.d/classifier /usr/local/bin/
rm -f file_transformation.zip
rm -f classifier_0.1_amd64.deb
nohup classifier_error_recovery > error.log 2>&1 &
tail /var/log/syslog
ls -lah /usr/local/classifier_static_data/
rm -f interactive_classifier.tar.gz
rm -f install_classifier.sh
rm -f classifier.dbg
