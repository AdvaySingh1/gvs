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

# 4. Clone DPDK repository
echo "Cloning DPDK repository..."
cd /tmp/gvs
sudo git clone https://github.com/DPDK/dpdk.git dpdk
cd dpdk
sudo git checkout v21.11  # or whatever version you need

# 5. Set up DPDK environment variables
echo "Setting up DPDK environment variables..."
echo 'export DPDK_BUILD=/tmp/gvs/dpdk/build' | sudo tee -a ~/.profile
echo 'export DPDK_BUILD=/tmp/gvs/dpdk/build' | sudo tee -a /root/.profile

# 6. Build and Install DPDK
echo "Building and installing DPDK..."
export DPDK_BUILD="/tmp/gvs/dpdk/build"
sudo meson -Denable_kmods=true -Denable_trace_fp=true build
sudo ninja -C build
sudo ninja -C build install
sudo ldconfig

# 7. Check DPDK version
echo "DPDK version:"
pkg-config --modversion libdpdk

# 8. Copy GVS to /tmp/gvs/gvs
echo "Copying GVS source..."
sudo cp -r ~/gigaflow/gvs /tmp/gvs/

# 9. Create build directory
sudo mkdir -p /tmp/gvs/gvs/build

# 10. Bootstrap
echo "Bootstrapping GVS..."
cd /tmp/gvs/gvs
sudo ./boot.sh

# 11. Configure with DPDK and GigaFlow
echo "Configuring GVS with DPDK and GigaFlow..."
cd /tmp/gvs/gvs/build
sudo .././configure --with-dpdk=static CC=gcc

# 12. Build
echo "Building GVS..."
sudo make -j4

# 13. Install
echo "Installing GVS..."
sudo make install

# 14. Set up environment variables
echo "Setting up environment variables..."
echo 'export PATH=$PATH:/usr/local/share/openvswitch/scripts' | sudo tee -a ~/.profile
echo 'export PATH=$PATH:/usr/local/share/openvswitch/scripts' | sudo tee -a /root/.profile
echo 'export DB_SOCK=/usr/local/var/run/openvswitch/db.sock' | sudo tee -a ~/.profile
echo 'export DB_SOCK=/usr/local/var/run/openvswitch/db.sock' | sudo tee -a /root/.profile

# 15. Source the profile
source ~/.profile

echo "=== GVS Installation Complete ==="
echo "DPDK location: /tmp/gvs/dpdk"
echo "DPDK build location: /tmp/gvs/dpdk/build"
echo "GVS build location: /tmp/gvs/gvs/build"
echo "Installation location: /usr/local"

echo ""
echo "To start OVS with DPDK, run:"
echo "bash .ci/linux-start-dpdk.sh" 