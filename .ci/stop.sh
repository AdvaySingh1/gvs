#!/bin/bash
export PATH="/usr/local/share/openvswitch/scripts:$PATH"
ovs-ctl stop
pkill -f ovs-vswitchd