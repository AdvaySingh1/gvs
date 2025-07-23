#!/bin/bash

set -e

# Configuration - adjust these as needed
HUGEPAGES_COUNT=1024
HUGEPAGES_SIZE_KB=2048
HUGEPAGES_TOTAL_SIZE="2G"
BRIDGE_NAME="br0"
DB_SOCK="/usr/local/var/run/openvswitch/db.sock"
ONIC_DEVICE_1="0000:00:10.0"
ONIC_DEVICE_2="0000:00:10.1"

# Set environment
export PATH="/usr/local/share/openvswitch/scripts:$PATH"
export DB_SOCK="$DB_SOCK"

echo "Setting up DPDK and OVS with OpenNIC..."

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
ovs-vsctl --all destroy Open_vSwitch || true
ovs-ctl --delete-bridges stop || true

# 3. Delete existing log file
rm -f /var/log/openvswitch/ovs-vswitchd.log || true

# 4. Check current device bindings and driver status
echo "Checking current device bindings..."
echo "OpenNIC devices found:"
lspci | grep -E '00:10\.[01]'
echo ""
echo "Driver status:"
lspci -k | grep -A 3 '00:10\.[01]'
echo ""

# Check if onic module is loaded
if lsmod | grep -q "onic"; then
    echo "✓ onic driver is loaded"
else
    echo "Loading onic driver..."
    modprobe onic || {
        echo "Error: Failed to load onic driver. Make sure OpenNIC driver is installed."
        exit 1
    }
fi

# 5. For OpenNIC with DPDK, we need to unbind from onic and bind to DPDK driver
echo "Setting up devices for DPDK..."

# First check if devices exist and are bound to onic
if lspci -k | grep -A 2 "$ONIC_DEVICE_1" | grep -q "onic"; then
    echo "Unbinding $ONIC_DEVICE_1 from onic driver..."
    echo "$ONIC_DEVICE_1" > /sys/bus/pci/drivers/onic/unbind || true
fi

if lspci -k | grep -A 2 "$ONIC_DEVICE_2" | grep -q "onic"; then
    echo "Unbinding $ONIC_DEVICE_2 from onic driver..."
    echo "$ONIC_DEVICE_2" > /sys/bus/pci/drivers/onic/unbind || true
fi

# Load UIO driver for DPDK
modprobe uio_pci_generic || {
    echo "Error: Failed to load uio_pci_generic driver"
    exit 1
}

# Bind devices to DPDK-compatible driver
echo "Binding devices to uio_pci_generic for DPDK..."
echo "$ONIC_DEVICE_1" > /sys/bus/pci/drivers/uio_pci_generic/bind || {
    echo "Failed to bind $ONIC_DEVICE_1 to uio_pci_generic"
}
echo "$ONIC_DEVICE_2" > /sys/bus/pci/drivers/uio_pci_generic/bind || {
    echo "Failed to bind $ONIC_DEVICE_2 to uio_pci_generic"
}

# Verify bindings
echo "Final device status:"
lspci -k | grep -A 3 '00:10\.[01]'

# 6. Start OVSDB and configure DPDK
echo "Starting OVSDB and configuring DPDK..."
ovs-ctl --no-ovs-vswitchd --system-id=random --delete-bridges start

# Configure OVS with DPDK and OpenNIC-specific settings
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

# 9. Add OpenNIC DPDK ports with correct interface names
echo "Adding OpenNIC DPDK ports..."
# Fixed: Use correct interface names that match the port names
ovs-vsctl add-port $BRIDGE_NAME dpdk0 -- set Interface dpdk0 type=dpdk options:dpdk-devargs=$ONIC_DEVICE_1
ovs-vsctl add-port $BRIDGE_NAME dpdk1 -- set Interface dpdk1 type=dpdk options:dpdk-devargs=$ONIC_DEVICE_2

echo "Setup complete! Bridge $BRIDGE_NAME created with OpenNIC DPDK ports."
echo ""
echo "Current bridge configuration:"
ovs-vsctl show

echo ""
echo "Useful commands:"
echo "  Check OVS logs: tail -f /usr/local/var/log/openvswitch/ovs-vswitchd.log"
echo "  Stop OVS: ovs-ctl stop"
echo "  Show bridge: ovs-vsctl show"
echo "  Check device bindings: lspci -k | grep -A 3 '00:10\.[01]'"
echo "  Re-bind to onic: echo '0000:00:10.0' > /sys/bus/pci/drivers/uio_pci_generic/unbind && echo '0000:00:10.0' > /sys/bus/pci/drivers/onic/bind"