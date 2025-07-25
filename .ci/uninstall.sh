#!/bin/bash

sed -i '/export PATH=.*:\/usr\/local\/share\/openvswitch\/scripts/d' ~/.profile
sed -i '/export PATH=.*:\/usr\/local\/share\/openvswitch\/scripts/d' /root/.profile

cd /tmp/gvs/ovs && make uninstall || true

apt-get remove -y make cmake llvm-12 clang-12 meson ninja-build autotools-dev autoconf libtool systemtap-sdt-dev

rm -rf /tmp/gvs
rm -rf /usr/local/etc/openvswitch /usr/local/var/run/openvswitch /usr/local/var/log/openvswitch