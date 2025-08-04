#!/bin/bash

export PATH="/usr/local/share/openvswitch/scripts:$PATH"

# Start OVS with userspace datapath
ovs-ctl --no-ovs-vswitchd --system-id=random --delete-bridges start

ovs-vsctl --no-wait \
  set Open_vSwitch . other_config:max-idle=600000 -- \
  set Open_vSwitch . other_config:n-handler-threads=1 -- \
  set Open_vSwitch . other_config:n-revalidator-threads=1 -- \
  set Open_vSwitch . other_config:gigaflow-enable=true -- \
  set Open_vSwitch . other_config:gigaflow-lookup-enable=true -- \
  set Open_vSwitch . other_config:gigaflow-debug-enable=false -- \
  set Open_vSwitch . other_config:gigaflow-tables-limit=5 -- \
  set Open_vSwitch . other_config:gigaflow-max-entries=20000 -- \
  set Open_vSwitch . other_config:gigaflow-max-masks=64 -- \
  set Open_vSwitch . other_config:gigaflow-optimize-paths=false -- \
  set Open_vSwitch . other_config:gigaflow-optimize-coupling=true -- \
  set Open_vSwitch . other_config:gigaflow-coupling-base-score=10 -- \
  set Open_vSwitch . other_config:gigaflow-coupling-scaler=100 -- \
  set Open_vSwitch . other_config:gigaflow-paths-scaler=1 -- \
  set Open_vSwitch . other_config:gigaflow-batch-state-updates=true -- \
  set Open_vSwitch . other_config:gigaflow-warmup-batches=10 -- \
  set Open_vSwitch . other_config:offload-gigaflow=true -- \
  set Open_vSwitch . other_config:hw-offload=true -- \
  set Open_vSwitch . other_config:hw-offload-p4sdnet=true

ovs-ctl --no-ovsdb-server --db-sock="$DB_SOCK" start

ovs-vsctl --if-exists del-br br0
ovs-vsctl add-br br0
ovs-vsctl set bridge br0 datapath_type=netdev
ovs-vsctl set bridge br0 fail-mode=secure
# ovs-vsctl add-port br0 p1
# ovs-vsctl add-port br0 p2

ovs-vsctl add-port br0 ens16f0
ovs-vsctl add-port br0 ens16f1


# Variables (edit as needed)
COLLECTOR_OVS_PATH="/tmp/gigaflow/logs/ee/high-locality/4-8000/cord-ofdpa"
RULES_PATH="/home/advay/gigaflow/ovs-pipelines"
RULESET="high-locality/cord/ofdpa/of2/10k/ruleset.ovs"

sudo rm -rf "$COLLECTOR_OVS_PATH"
sudo mkdir -p "$COLLECTOR_OVS_PATH"
# sudo ovs-ofctl add-flows br0 "$RULES_PATH/$RULESET"


  ovs-ofctl add-flow br0 "table=0, priority=100, dl_dst=00:00:00:00:00:05, actions=output:ens16f0"
  ovs-ofctl add-flow br0 "table=0, priority=100, dl_dst=00:00:00:00:00:06, actions=output:ens16f1"