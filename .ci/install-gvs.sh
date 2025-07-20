#!/bin/bash
set -e

echo "=== GVS Installation Script ==="

# RESET EVERYTHING (run this first to clean up)
echo "Cleaning up previous installations..."
sudo rm -rf /tmp/gvs ~/gigaflow/gvs/dpdk-dir ~/gigaflow/gvs/dpdk-*.tar.xz ~/gigaflow/gvs/build
sudo apt remove --purge openvswitch* -y 2>/dev/null || true
sudo rm -rf /usr/local/share/openvswitch /usr/local/var/run/openvswitch /usr/local/var/log/openvswitch
sudo sed -i '/openvswitch/d' ~/.profile /root/.profile 2>/dev/null || true

# FRESH SETUP (equivalent to your Ansible)
echo "Setting up fresh installation..."

# 1. Create project directory
sudo mkdir -p /tmp/gvs

# 2. Install dependencies
echo "Installing dependencies..."
sudo apt update
sudo apt install -y make cmake llvm-12 clang-12 meson ninja-build python3-pip pkg-config autotools-dev autoconf libtool systemtap-sdt-dev python-setuptools

# 3. Install pyelftools
echo "Installing pyelftools..."
pip3 install --user --upgrade pyelftools

# 4. Copy GVS to /tmp/gvs/gvs
echo "Copying GVS source..."
sudo cp -r ~/gigaflow/gvs /tmp/gvs/

# 5. Create build directory
sudo mkdir -p /tmp/gvs/gvs/build

# 6. Bootstrap
echo "Bootstrapping GVS..."
cd /tmp/gvs/gvs
sudo ./boot.sh

# 7. Configure with DPDK and GigaFlow
echo "Configuring GVS with DPDK and GigaFlow..."
cd /tmp/gvs/gvs/build
sudo .././configure --with-dpdk=static CC=gcc

# 8. Build
echo "Building GVS..."
sudo make -j4

# 9. Install
echo "Installing GVS..."
sudo make install

# 10. Set up environment variables
echo "Setting up environment variables..."
echo 'export PATH=$PATH:/usr/local/share/openvswitch/scripts' | sudo tee -a ~/.profile
echo 'export PATH=$PATH:/usr/local/share/openvswitch/scripts' | sudo tee -a /root/.profile
echo 'export DB_SOCK=/usr/local/var/run/openvswitch/db.sock' | sudo tee -a ~/.profile
echo 'export DB_SOCK=/usr/local/var/run/openvswitch/db.sock' | sudo tee -a /root/.profile

# 11. Source the profile
source ~/.profile

echo "=== GVS Installation Complete ==="
echo "DPDK location: /tmp/gvs/gvs/dpdk-dir"
echo "Build location: /tmp/gvs/gvs/build"
echo "Installation location: /usr/local"

echo ""
echo "To start OVS with DPDK, run:"
echo "bash .ci/linux-start-dpdk.sh" 