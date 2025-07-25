#!/bin/bash

set -e  # Exit on any error

PROJECT_NAME="gvs"

echo "Starting OVS uninstallation..."

# Stop OVS services if running
echo "Stopping OVS services..."
pkill -f ovs-vswitchd || true
pkill -f ovsdb-server || true

# Remove OVS scripts from PATH in profiles
echo "Removing OVS scripts from PATH..."
if [ -f ~/.profile ]; then
    sed -i '/^export PATH=.*:\/usr\/local\/share\/openvswitch\/scripts.*$/d' ~/.profile
fi
if [ -f /root/.profile ]; then
    sed -i '/^export PATH=.*:\/usr\/local\/share\/openvswitch\/scripts.*$/d' /root/.profile
fi

# Uninstall OVS if source directory exists
if [ -d "/tmp/${PROJECT_NAME}/ovs" ]; then
    echo "Uninstalling OVS..."
    cd /tmp/${PROJECT_NAME}/ovs
    make uninstall || echo "Warning: make uninstall failed, continuing..."
else
    echo "Warning: OVS source directory not found, skipping make uninstall"
fi

# Remove OVS binaries and files manually (in case make uninstall didn't work)
echo "Removing OVS files manually..."
rm -rf /usr/local/bin/ovs-*
rm -rf /usr/local/sbin/ovs-*
rm -rf /usr/local/bin/ovsdb-*
rm -rf /usr/local/sbin/ovsdb-*
rm -rf /usr/local/share/openvswitch
rm -rf /usr/local/etc/openvswitch
rm -rf /usr/local/var/run/openvswitch
rm -rf /usr/local/var/log/openvswitch
rm -rf /usr/local/include/openvswitch
rm -rf /usr/local/lib/libopenvswitch*
rm -rf /usr/local/lib/libovsdb*

# Remove OVS dependencies (optional - uncomment if you want to remove them)
echo "Removing OVS dependencies..."
# Uncomment the following lines if you want to remove the dependencies
# apt-get remove -y make cmake llvm-12 clang-12 meson ninja-build
# apt-get remove -y autotools-dev autoconf libtool
# apt-get remove -y systemtap-sdt-dev
# apt-get autoremove -y

# Remove LLVM repository (optional)
echo "Removing LLVM repository..."
rm -f /etc/apt/sources.list.d/llvm-toolchain-xenial-12.list
apt-get update || true

# Remove temporary directory
echo "Removing temporary directory..."
rm -rf /tmp/${PROJECT_NAME}

# Remove any remaining OVS kernel modules
echo "Removing OVS kernel modules..."
rmmod openvswitch || true
rmmod vport_geneve || true
rmmod vport_gre || true
rmmod vport_vxlan || true

echo "OVS uninstallation completed!"
echo "Note: Some dependencies were left installed. Uncomment lines in script to remove them."
echo "Please restart your shell or run 'source ~/.profile' to update PATH"
