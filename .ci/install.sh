#!/bin/bash

set -e

apt-get update

mkdir -p /tmp/gvs
rm -rf /tmp/gvs/ovs
git clone https://github.com/openvswitch/ovs.git /tmp/gvs/ovs
cd /tmp/gvs/ovs
git checkout main

printf "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-12 main" | tee /etc/apt/sources.list.d/llvm-toolchain-focal-12.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
apt-get update
apt-get install -y make cmake llvm-12 clang-12 meson ninja-build autotools-dev autoconf libtool systemtap-sdt-dev

./boot.sh
./configure
make -j4
make install

echo "export PATH=\$PATH:/usr/local/share/openvswitch/scripts" >> ~/.profile
echo "export PATH=\$PATH:/usr/local/share/openvswitch/scripts" >> /root/.profile
export PATH=$PATH:/usr/local/share/openvswitch/scripts

mkdir -p /usr/local/etc/openvswitch /usr/local/var/run/openvswitch /usr/local/var/log/openvswitch