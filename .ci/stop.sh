#!/bin/bash

# Set PATH to include OVS scripts
export PATH="/usr/local/share/openvswitch/scripts:$PATH"

# Stop OVS
ovs-ctl stop