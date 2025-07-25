#!/bin/bash

set -e  # Exit on any error

PROJECT_NAME="gvs"
OVS_REPO="https://github.com/openvswitch/ovs.git"
OVS_VERSION="master"  # Change this to specific version if needed

echo "Starting OVS installation..."

# Update package cache
echo "Updating package cache..."
apt-get update

# Create temporary directory
echo "Creating temporary directory..."
mkdir -p /tmp/${PROJECT_NAME}

# Clone OVS repository
echo "Cloning OVS repository..."
if [ -d "/tmp/${PROJECT_NAME}/ovs" ]; then
    echo "OVS directory already exists, removing..."
    rm -rf /tmp/${PROJECT_NAME}/ovs
fi
git clone ${OVS_REPO} /tmp/${PROJECT_NAME}/ovs
cd /tmp/${PROJECT_NAME}/ovs
git checkout ${OVS_VERSION}

# Install OVS dependencies
echo "Installing OVS dependencies..."
printf "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-12 main" | tee /etc/apt/sources.list.d/llvm-toolchain-xenial-12.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
apt-get update
apt-get install -y make cmake llvm-12 clang-12 meson ninja-build
apt-get install -y autotools-dev autoconf libtool
apt-get install -y systemtap-sdt-dev

# Build OVS with debug symbols and no optimization (to fix segfault issues)
echo "Building OVS..."
cd /tmp/${PROJECT_NAME}/ovs
./boot.sh
./configure --enable-debug CFLAGS="-g -O0"
make -j4

# Install OVS
echo "Installing OVS..."
make install

# Add OVS scripts to PATH in profiles
echo "Adding OVS scripts to PATH..."
echo "export PATH=\$PATH:/usr/local/share/openvswitch/scripts" >> ~/.profile
echo "export PATH=\$PATH:/usr/local/share/openvswitch/scripts" >> /root/.profile

# Create OVS directories
echo "Creating OVS directories..."
mkdir -p /usr/local/etc/openvswitch
mkdir -p /usr/local/var/run/openvswitch
mkdir -p /usr/local/var/log/openvswitch

echo "OVS installation completed successfully!"
echo "Please run 'source ~/.profile' or restart your shell to update PATH"
