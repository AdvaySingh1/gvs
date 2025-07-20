#!/bin/bash

set -e

# Configuration - adjust these as needed
HUGEPAGES_COUNT=1024
HUGEPAGES_SIZE_KB=2048
HUGEPAGES_TOTAL_SIZE="2G"
BRIDGE_NAME="br0"
DB_SOCK="/usr/local/var/run/openvswitch/db.sock"

# Set environment
export PATH="/usr/local/share/openvswitch/scripts:$PATH"
export DB_SOCK="$DB_SOCK"

echo "Setting up DPDK and OVS..."

# 1. Configure and mount HugePages
echo "Configuring hugepages..."
echo $HUGEPAGES_COUNT > /sys/kernel/mm/hugepages/hugepages-${HUGEPAGES_SIZE_KB}kB/nr_hugepages

# Use DPDK hugepages script if available
if [ -f "/tmp/gvs/dpdk/usertools/dpdk-hugepages.py" ]; then
    /tmp/gvs/dpdk/usertools/dpdk-hugepages.py -p ${HUGEPAGES_SIZE_KB}K --setup $HUGEPAGES_TOTAL_SIZE
fi

# Check hugepages mount
echo "Checking hugepages mount:"
mount | grep hugetlbfs || echo "No hugetlbfs mount found"

# Show DPDK hugepage status
if [ -f "/tmp/gvs/dpdk/usertools/dpdk-hugepages.py" ]; then
    echo "DPDK hugepage status:"
    /tmp/gvs/dpdk/usertools/dpdk-hugepages.py -s
fi

# 2. Stop existing OVS instances
echo "Stopping existing OVS instances..."
/usr/local/share/openvswitch/scripts/ovs-vsctl --all destroy Open_vSwitch || true
/usr/local/share/openvswitch/scripts/ovs-ctl --delete-bridges stop || true

# 3. Delete existing log file
rm -f /var/log/openvswitch/ovs-vswitchd.log || true

# 4. Load required modules for Xilinx DPDK binding
echo "Loading required modules..."
modprobe uio_pci_generic || true
modprobe onic || true

# 5. Bind Xilinx devices to DPDK
echo "Binding Xilinx devices to DPDK..."
if [ -f "/tmp/gvs/dpdk/usertools/dpdk-devbind.py" ]; then
    python3 /tmp/gvs/dpdk/usertools/dpdk-devbind.py -b uio_pci_generic 0000:00:10.0
    python3 /tmp/gvs/dpdk/usertools/dpdk-devbind.py -b uio_pci_generic 0000:00:10.1
fi

# 6. Start OVSDB and configure DPDK
echo "Starting OVSDB and configuring DPDK..."
ovs-ctl --no-ovs-vswitchd --system-id=random --delete-bridges start

ovs-vsctl --no-wait \
    set Open_vSwitch . other_config:dpdk-init=true -- \
    set Open_vSwitch . other_config:max-idle=500 -- \
    set Open_vSwitch . other_config:gigaflow-enable=true -- \
    set Open_vSwitch . other_config:gigaflow-lookup-enable=true -- \
    set Open_vSwitch . other_config:gigaflow-tables-limit=1000 -- \
    set Open_vSwitch . other_config:gigaflow-max-entries=1000000 -- \
    set Open_vSwitch . other_config:gigaflow-max-masks=1000 -- \
    set Open_vSwitch . other_config:gigaflow-estimate-flow-space=true -- \
    set Open_vSwitch . other_config:hw-offload=false -- \
    set Open_vSwitch . other_config:hw-offload-p4sdnet=false -- \
    set Open_vSwitch . other_config:gigaflow-offload=false -- \
    set Open_vSwitch . other_config:n-handler-threads=1 -- \
    set Open_vSwitch . other_config:n-revalidator-threads=1 -- \
    set Open_vSwitch . other_config:pmd-perf-metrics=true

# 7. Start GVS vswitchd
echo "Starting GVS vswitchd..."
ovs-ctl --no-ovsdb-server --db-sock="$DB_SOCK" start

# 8. Create bridge
echo "Creating bridge $BRIDGE_NAME..."
ovs-vsctl del-br $BRIDGE_NAME || true
ovs-vsctl add-br $BRIDGE_NAME -- set bridge $BRIDGE_NAME datapath_type=netdev fail-mode=secure

# 9. Add Xilinx DPDK ports
echo "Adding Xilinx DPDK ports..."
ovs-vsctl add-port $BRIDGE_NAME dpdk0 -- set Interface dpdk0 type=dpdk options:dpdk-devargs=0000:00:10.0
ovs-vsctl add-port $BRIDGE_NAME dpdk1 -- set Interface dpdk1 type=dpdk options:dpdk-devargs=0000:00:10.1

echo "Setup complete! Bridge $BRIDGE_NAME created with Xilinx DPDK ports."
echo ""
echo "Current bridge configuration:"
ovs-vsctl show 