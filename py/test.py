#!/usr/bin/env python3
import os
import time

from scapy.all import Ether, Dot1Q, IP, TCP, UDP, sendp
import socket
import struct


def generate_packet(
    eth_src="00:00:00:00:00:00",
    eth_dst="00:00:00:00:00:00",
    eth_type=0x0800,  # Fixed: Default to IPv4
    vlan_vid=0x000,
    vlan_pcp=0x0,
    ipv4_src="0x00000000",  # will convert to dotted decimal
    ipv4_dst="0x00000000",  # will convert to dotted decimal
    ipv4_proto=0x00,
    ipv4_tos=0x00,
    l4_sport=0x0000,
    l4_dport=0x0000,
):
    """
    Generate an Ethernet + optional VLAN + IPv4 + TCP/UDP packet.

    Returns:
        pkt: Scapy packet object (Ether/802.1Q/IP/TCP or UDP)
    """

    def hex_ip_to_str(ip_hex):
        return socket.inet_ntoa(struct.pack("!I", int(ip_hex, 16)))

    # Fixed: Don't override eth_type when VLAN is present
    if vlan_vid != 0:
        eth_layer = Ether(src=eth_src, dst=eth_dst, type=0x8100)  # VLAN tag
        vlan_layer = Dot1Q(vlan=vlan_vid, prio=vlan_pcp, type=eth_type)  # Inner type
    else:
        eth_layer = Ether(src=eth_src, dst=eth_dst, type=eth_type)
        vlan_layer = None

    # Fixed: Ensure IPv4 header is properly constructed
    ip_layer = IP(
        src=hex_ip_to_str(ipv4_src),
        dst=hex_ip_to_str(ipv4_dst),
        proto=ipv4_proto,
        tos=ipv4_tos,
        version=4,  # Explicitly set IPv4 version
        ihl=5,      # Explicitly set header length (5 * 4 = 20 bytes)
        ttl=64      # Set reasonable TTL
    )

    # Fixed: Handle L4 layer construction
    if ipv4_proto == 0x11:  # UDP
        l4_layer = UDP(sport=l4_sport, dport=l4_dport)
    elif ipv4_proto == 0x06:  # TCP
        l4_layer = TCP(sport=l4_sport, dport=l4_dport)
    elif ipv4_proto == 0x00:  # No L4 protocol
        l4_layer = None
    else:
        raise ValueError(f"Unsupported IPv4 proto: {ipv4_proto:#02x}")

    # Fixed: Proper packet construction
    if vlan_layer and l4_layer:
        pkt = eth_layer / vlan_layer / ip_layer / l4_layer
    elif vlan_layer:
        pkt = eth_layer / vlan_layer / ip_layer
    elif l4_layer:
        pkt = eth_layer / ip_layer / l4_layer
    else:
        pkt = eth_layer / ip_layer

    return pkt


IFACE = "ens16f0"

def main():
    # === Generate 3 distinct packets ===
    
    # Packet 1: SHOULD MATCH - destination IP 10.0.0.1 (matches your C rule)
    pkt1 = generate_packet(
        eth_src="aa:bb:cc:dd:ee:ff",
        eth_dst="00:0a:35:00:10:00",
        eth_type=0x0800,
        vlan_vid=0x0,
        vlan_pcp=0x0,
        ipv4_src="0x0c0d0e0f",   # 12.13.14.15 (source doesn't matter - wildcarded)
        ipv4_dst="0x0a000001",   # 10.0.0.1 - MATCHES YOUR RULE!
        ipv4_proto=0x11,         # UDP (protocol doesn't matter - wildcarded)
        ipv4_tos=0x00,
        l4_sport=0x04d2,         # 1234 (sport doesn't matter - wildcarded)
        l4_dport=0x10e1          # 4321 (dport doesn't matter - wildcarded)
    )
    
    # Packet 2: SHOULD NOT MATCH - different destination IP
    pkt2 = generate_packet(
        eth_src="aa:bb:cc:dd:ee:ff",
        eth_dst="00:0a:35:00:10:00",
        eth_type=0x0800,
        vlan_vid=0x0,
        vlan_pcp=0x0,
        ipv4_src="0x0a000002",   # 10.0.0.2
        ipv4_dst="0x0a000002",   # 10.0.0.2 - DOES NOT MATCH (not 10.0.0.1)
        ipv4_proto=0x11,         # UDP
        ipv4_tos=0x00,
        l4_sport=0x04d2,         # 1234
        l4_dport=0x10e1          # 4321
    )
    
    # Packet 3: SHOULD NOT MATCH - different destination IP
    pkt3 = generate_packet(
        eth_src="aa:bb:cc:dd:ee:ff",
        eth_dst="00:0a:35:00:10:00",
        eth_type=0x0800,
        vlan_vid=0x0,
        vlan_pcp=0x0,
        ipv4_src="0x0c0d0e64",   # 12.13.14.100
        ipv4_dst="0x0a000003",   # 10.0.0.3 - DOES NOT MATCH (not 10.0.0.1)
        ipv4_proto=0x11,         # UDP
        ipv4_tos=0x00,
        l4_sport=0x04d2,         # 1234
        l4_dport=0x10e1          # 4321
    )
    
    # === Send all 3 packets ===
    for i, pkt in enumerate([pkt1, pkt2, pkt3], 1):
        if i == 1:
            print(f"\n== Sending pkt{i} (SHOULD MATCH: dst=10.0.0.1 -> forward to port 4) ==")
            print(f"   Source: {pkt[IP].src}, Destination: {pkt[IP].dst}")
        elif i == 2:
            print(f"\n== Sending pkt{i} (SHOULD NOT MATCH: dst=10.0.0.2 -> default action) ==")
            print(f"   Source: {pkt[IP].src}, Destination: {pkt[IP].dst}")
        else:
            print(f"\n== Sending pkt{i} (SHOULD NOT MATCH: dst=10.0.0.3 -> default action) ==")
            print(f"   Source: {pkt[IP].src}, Destination: {pkt[IP].dst}")
        
        time.sleep(0.3)
        sendp(pkt, iface=IFACE, verbose=False)
        print(f"   Packet {i} sent!")

if __name__ == "__main__":
    main()