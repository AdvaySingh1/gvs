#!/bin/bash

set -e

# Configuration - adjust these as needed
BRIDGE_NAME="br0"
DB_SOCK="/usr/local/var/run/openvswitch/db.sock"
ONIC_INTERFACE_1="ens16f0"  # First OpenNIC interface
ONIC_INTERFACE_2="ens16f1"  # Second OpenNIC interface

# Set environment
export PATH="/usr/local/share/openvswitch/scripts:$PATH"
export DB_SOCK="$DB_SOCK"

echo "Setting up kernel mode OVS with OpenNIC interfaces..."

# 1. Check if onic interfaces exist
echo "Checking OpenNIC interfaces..."
if ! ip link show $ONIC_INTERFACE_1 > /dev/null 2>&1; then
    echo "Error: Interface $ONIC_INTERFACE_1 not found"
    echo "Available interfaces:"
    ip link show | grep -E '^[0-9]+:' | cut -d: -f2 | sed 's/^ //'
    exit 1
fi

if ! ip link show $ONIC_INTERFACE_2 > /dev/null 2>&1; then
    echo "Error: Interface $ONIC_INTERFACE_2 not found"
    echo "Available interfaces:"
    ip link show | grep -E '^[0-9]+:' | cut -d: -f2 | sed 's/^ //'
    exit 1
fi

echo "✓ Found OpenNIC interfaces: $ONIC_INTERFACE_1, $ONIC_INTERFACE_2"

# 2. Verify onic driver is loaded and interfaces are using it
echo "Verifying onic driver status..."
if ! lsmod | grep -q "onic"; then
    echo "Loading onic driver..."
    sudo modprobe onic || {
        echo "Error: Failed to load onic driver. Make sure OpenNIC driver is installed."
        exit 1
    }
fi

# Check driver binding
echo "Interface driver information:"
ethtool -i $ONIC_INTERFACE_1 | grep driver || echo "Could not get driver info for $ONIC_INTERFACE_1"
ethtool -i $ONIC_INTERFACE_2 | grep driver || echo "Could not get driver info for $ONIC_INTERFACE_2"

# 3. Stop existing OVS instances
echo "Stopping existing OVS instances..."
sudo ovs-vsctl --all destroy Open_vSwitch 2>/dev/null || true
sudo /tmp/gvs/gvs/build/utilities/ovs-ctl --delete-bridges stop 2>/dev/null || true

# 4. Delete existing log files
sudo rm -f /usr/local/var/log/openvswitch/ovs-vswitchd.log || true

# 5. Bring up the OpenNIC interfaces
echo "Configuring OpenNIC interfaces..."
sudo ip link set $ONIC_INTERFACE_1 up
sudo ip link set $ONIC_INTERFACE_2 up

# Wait a moment for interfaces to come up
sleep 2

# Check interface status
echo "Interface status:"
ip link show $ONIC_INTERFACE_1 | grep -E "(UP|DOWN)"
ip link show $ONIC_INTERFACE_2 | grep -E "(UP|DOWN)"

# 6. Start OVSDB 
echo "Starting OVSDB..."
sudo /tmp/gvs/gvs/build/utilities/ovs-ctl --no-ovs-vswitchd --system-id=random --delete-bridges start

# 7. Configure OVS for kernel mode with GigaFlow
echo "Configuring OVS for kernel mode with GigaFlow..."
sudo ovs-vsctl --no-wait \
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
    set Open_vSwitch . other_config:n-revalidator-threads=1

# 8. Start GVS vswitchd in kernel mode
echo "Starting GVS vswitchd in kernel mode..."
sudo /tmp/gvs/gvs/build/utilities/ovs-ctl --no-ovsdb-server --db-sock="$DB_SOCK" start

# 9. Create bridge (kernel datapath is default)
echo "Creating bridge $BRIDGE_NAME..."
sudo ovs-vsctl del-br $BRIDGE_NAME 2>/dev/null || true
sudo ovs-vsctl add-br $BRIDGE_NAME -- set bridge $BRIDGE_NAME fail-mode=secure

# 10. Add OpenNIC interfaces as regular ports
echo "Adding OpenNIC interfaces to bridge..."
sudo ovs-vsctl add-port $BRIDGE_NAME $ONIC_INTERFACE_1
sudo ovs-vsctl add-port $BRIDGE_NAME $ONIC_INTERFACE_2

# 11. Verify configuration
echo ""
echo "Setup complete! Bridge $BRIDGE_NAME created with OpenNIC kernel interfaces."
echo ""
echo "Current bridge configuration:"
sudo ovs-vsctl show

echo ""
echo "Bridge ports status:"
sudo ovs-vsctl list port

echo ""
echo "Interface status:"
ip link show $ONIC_INTERFACE_1 | grep -E "state (UP|DOWN)"
ip link show $ONIC_INTERFACE_2 | grep -E "state (UP|DOWN)"

echo ""
echo "Useful commands:"
echo "  Check OVS logs: sudo tail -f /usr/local/var/log/openvswitch/ovs-vswitchd.log"
echo "  Stop OVS: sudo /tmp/gvs/gvs/build/utilities/ovs-ctl stop"
echo "  Show bridge: sudo ovs-vsctl show"
echo "  Show flows: sudo ovs-ofctl dump-flows $BRIDGE_NAME"
echo "  Interface stats: sudo ovs-vsctl get interface $ONIC_INTERFACE_1 statistics"
echo "  Check interface status: ip addr show $ONIC_INTERFACE_1"