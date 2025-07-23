#!/bin/bash

# Set PATH to include OVS scripts
export PATH="/usr/local/share/openvswitch/scripts:$PATH"

# Start OVS
ovs-ctl --system-id=random --delete-bridges start
ovs-vsctl set Open_vSwitch . other_config:max-idle=10000

# Bring up network interfaces
ifconfig ens16f0 up
ifconfig ens16f1 up

# Add bridge br0 to OVS
ovs-vsctl add-br br0
ovs-vsctl set-fail-mode br0 secure

# Add ports to br0
ovs-vsctl add-port br0 ens16f0
ovs-vsctl add-port br0 ens16f1