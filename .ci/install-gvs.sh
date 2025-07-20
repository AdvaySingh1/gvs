#!/bin/bash
set -e

echo "=== GVS Installation Script ==="

# RESET EVERYTHING (run this first to clean up)
echo "Cleaning up previous installations..."
sudo rm -rf /tmp/gvs ~/gigaflow/gvs/dpdk-dir ~/gigaflow/gvs/dpdk-*.tar.xz ~/gigaflow/gvs/build
sudo apt remove --purge openvswitch* -y 2>/dev/null || true
sudo rm -rf /usr/local/share/openvswitch /usr/local/var/run/openvswitch /usr/local/var/log/openvswitch
sudo sed -i '/openvswitch/d' ~/.profile /root/.profile 2>/dev/null || true
sudo sed -i '/DPDK_BUILD/d' ~/.profile /root/.profile 2>/dev/null || true
sudo sed -i '/DB_SOCK/d' ~/.profile /root/.profile 2>/dev/null || true

# FRESH SETUP (equivalent to your Ansible)
echo "Setting up fresh installation..."

# 1. Create project directory
sudo mkdir -p /tmp/gvs

# 2. Install LLVM GPG key and repository (matching Ansible)
echo "Setting up LLVM repository..."
curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | sudo gpg --dearmor -o /usr/share/keyrings/llvm-archive-keyring.gpg
echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/focal/ llvm-toolchain-focal-12 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-focal-12.list

# 3. Install dependencies (exactly matching Ansible)
echo "Installing dependencies..."
sudo apt update
sudo apt install -y make cmake llvm-12 clang-12 meson ninja-build python3-pip pkg-config autotools-dev autoconf libtool systemtap-sdt-dev python-setuptools

# 4. Install pyelftools
echo "Installing pyelftools..."
sudo pip3 install --upgrade pyelftools

# Remember the original directory (where the script was called from)
ORIGINAL_DIR=$(pwd)

# 5. Clone DPDK repository (matching the DPDK install tasks)
echo "Cloning DPDK repository..."
cd /tmp/gvs
# Note: You'll need to replace these with actual values from your config
# sudo git clone "{{ gvs.dpdk.git.repo }}" dpdk
# cd dpdk
# sudo git checkout "{{ gvs.dpdk.git.version }}"
# For now, using standard DPDK repo - update these lines with your actual repo and version:
sudo git clone https://github.com/DPDK/dpdk.git dpdk
cd dpdk
sudo git checkout v21.11  # Replace with actual version from your config

# 6. Set up DPDK environment variables (matching Ansible lineinfile tasks)
echo "Setting up DPDK environment variables..."
echo 'export DPDK_BUILD=/tmp/gvs/dpdk/build' >> ~/.profile
echo 'export DPDK_BUILD=/tmp/gvs/dpdk/build' | sudo tee -a /root/.profile > /dev/null

# 7. Build and Install DPDK (matching Ansible block with environment)
echo "Building and installing DPDK..."
cd /tmp/gvs/dpdk
export DPDK_BUILD="/tmp/gvs/dpdk/build"

# Configure DPDK build with meson
meson -Denable_kmods=true -Denable_trace_fp=true build

# Compile DPDK with ninja
ninja -C build

# Install DPDK with ninja
sudo ninja -C build install

# Update shared library cache
sudo ldconfig

# Check DPDK version
echo "DPDK version:"
pkg-config --modversion libdpdk

# 8. Copy GVS source from original directory to /tmp/gvs/gvs
echo "Copying GVS source..."
# Verify boot.sh exists in the original directory
if [ ! -f "$ORIGINAL_DIR/boot.sh" ]; then
    echo "Error: boot.sh not found in original directory ($ORIGINAL_DIR)"
    echo "Please run this script from the GVS source directory (the one containing boot.sh)"
    exit 1
fi

# Copy the GVS source to /tmp/gvs/gvs
sudo cp -r "$ORIGINAL_DIR" /tmp/gvs/gvs

# 9. Create build directory
sudo mkdir -p /tmp/gvs/gvs/build

# 10. Bootstrap
echo "Bootstrapping GVS..."
cd /tmp/gvs/gvs
sudo ./boot.sh

# 11. Configure with DPDK (matching Ansible exactly)
echo "Configuring GVS with DPDK..."
cd /tmp/gvs/gvs
# Clean any previous configuration from source directory
sudo make distclean 2>/dev/null || true
cd /tmp/gvs/gvs/build
sudo .././configure --with-dpdk=static CC=gcc

# 12. Build
echo "Building GVS..."
sudo make -j4

# 13. Install
echo "Installing GVS..."
sudo make install

echo "Setting up environment variables..."
echo 'export PATH=$PATH:/usr/local/share/openvswitch/scripts' >> ~/.profile
echo 'export PATH=$PATH:/usr/local/share/openvswitch/scripts' | sudo tee -a /root/.profile > /dev/null
echo 'export DB_SOCK=/usr/local/var/run/openvswitch/db.sock' >> ~/.profile
echo 'export DB_SOCK=/usr/local/var/run/openvswitch/db.sock' | sudo tee -a /root/.profile > /dev/null

source ~/.profile

echo "=== GVS Installation Complete ==="
echo "DPDK location: /tmp/gvs/dpdk"
echo "DPDK build location: /tmp/gvs/dpdk/build"
echo "GVS build location: /tmp/gvs/gvs/build"
echo "Installation location: /usr/local"

echo ""
echo "To start OVS with DPDK, run:"
echo "bash .ci/linux-start-dpdk.sh"