#!/bin/bash
set -e

# --- CONFIGURATION ---
# Set these to your actual PCI addresses for ens16f0 and ens16f1
PCI_ADDR_0="0000:00:10.0"
PCI_ADDR_1="0000:00:10.1"

# Path to DPDK devbind script (update if your DPDK is elsewhere)
DPDK_BIND="/path/to/dpdk/usertools/dpdk-devbind.py"  # TODO: Update this path

# Number of hugepages and size
HUGEPAGES=1024
HUGEPAGE_SIZE_KB=2048

# --- SETUP HUGEPAGES ---
echo "$HUGEPAGES" | sudo tee /sys/kernel/mm/hugepages/hugepages-${HUGEPAGE_SIZE_KB}kB/nr_hugepages
sudo mkdir -p /dev/hugepages
sudo mount -t hugetlbfs none /dev/hugepages || true

# --- BIND INTERFACES TO vfio-pci ---
sudo modprobe vfio-pci
sudo $DPDK_BIND --bind=vfio-pci $PCI_ADDR_0 $PCI_ADDR_1
sudo $DPDK_BIND --status

# --- SETUP ENVIRONMENT ---
export PATH=$PATH:/usr/local/share/openvswitch/scripts
export DB_SOCK=/usr/local/var/run/openvswitch/db.sock

# --- START OVS WITH DPDK ---
sudo ovs-ctl --no-ovs-vswitchd --system-id=random --delete-bridges start
sudo ovs-vsctl --no-wait set Open_vSwitch . other_config:dpdk-init=true
sudo ovs-ctl --no-ovsdb-server --db-sock="$DB_SOCK" start

# --- CREATE BRIDGE AND ADD PORTS ---
sudo ovs-vsctl add-br br0 -- set bridge br0 datapath_type=netdev
sudo ovs-vsctl add-port br0 dpdk0 -- set Interface dpdk0 type=dpdk options:dpdk-devargs=$PCI_ADDR_0
sudo ovs-vsctl add-port br0 dpdk1 -- set Interface dpdk1 type=dpdk options:dpdk-devargs=$PCI_ADDR_1

# --- CLEAR PMD STATS AND SHOW CONFIG ---
sudo ovs-appctl dpif-netdev/pmd-stats-clear
sudo ovs-vsctl show

echo "GVS DPDK start and bridge/port setup complete." 