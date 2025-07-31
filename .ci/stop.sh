#!/bin/bash
export PATH="/usr/local/share/openvswitch/scripts:$PATH"
ovs-ctl stop
pkill -f ovs-vswitchd

sudo ip link delete ovs-netdev
sudo ip link delete br0