#!/bin/bash

export PATH="/usr/local/share/openvswitch/scripts:$PATH"

# Start OVS with userspace datapath
ovs-ctl --system-id=random --delete-bridges start
ovs-vsctl set Open_vSwitch . other_config:max-idle=600000 -- \
  set Open_vSwitch . other_config:gigaflow-enable=true -- \
  set Open_vSwitch . other_config:gigaflow-lookup-enable=true -- \
  set Open_vSwitch . other_config:gigaflow-tables-limit=4 -- \
  set Open_vSwitch . other_config:gigaflow-max-entries=8000 -- \
  set Open_vSwitch . other_config:gigaflow-max-masks=64 -- \
  set Open_vSwitch . other_config:gigaflow-estimate-flow-space=true -- \
  set Open_vSwitch . other_config:hw-offload=true -- \
  set Open_vSwitch . other_config:hw-offload-p4sdnet=true -- \
  set Open_vSwitch . other_config:gigaflow-offload=true -- \
  set Open_vSwitch . other_config:n-handler-threads=1 -- \
  set Open_vSwitch . other_config:n-revalidator-threads=1 -- \
  set Open_vSwitch . other_config:pmd-perf-metrics=true


ovs-vsctl add-br br0
ovs-vsctl set-fail-mode br0 secure
# ovs-vsctl add-port br0 ens16f0
# ovs-vsctl add-port br0 ens16f1