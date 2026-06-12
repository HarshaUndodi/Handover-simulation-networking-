#!/usr/bin/env python3
import subprocess
import json
import argparse
import sys
import re

def run_command(command):
    try:
        result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        return e.stdout + e.stderr

def get_interface_ip(interface, netns=None):
    prefix = f"sudo ip netns exec {netns} " if netns else ""
    try:
        result = subprocess.run(f"{prefix}ip -4 addr show {interface}", shell=True, check=True, stdout=subprocess.PIPE, text=True)
        match = re.search(r'inet\s+(\d+\.\d+\.\d+\.\d+)', result.stdout)
        if match:
            return match.group(1)
        return None
    except subprocess.CalledProcessError:
        return None

def start_iperf3_server(server_ip):
    # Check if iperf3 is already running
    check_cmd = "pgrep iperf3"
    try:
        subprocess.run(check_cmd, shell=True, check=True, stdout=subprocess.PIPE)
        print("[*] iperf3 server is already running.")
    except subprocess.CalledProcessError:
        print(f"[*] Starting iperf3 server on {server_ip}...")
        subprocess.run(f"iperf3 -s -B {server_ip} -D", shell=True)

def measure_ping(interface, target_ip, count=10, netns=None):
    print(f"[*] Measuring Latency and Packet Loss (pinging {target_ip} from {interface})...")
    prefix = f"sudo ip netns exec {netns} " if netns else ""
    cmd = f"{prefix}ping -I {interface} {target_ip} -c {count}"
    output = run_command(cmd)
    
    latency = "N/A"
    packet_loss = "N/A"
    
    loss_match = re.search(r'(\d+)% packet loss', output)
    if loss_match:
        packet_loss = f"{loss_match.group(1)}%"
        
    rtt_match = re.search(r'rtt min/avg/max/mdev = [\d\.]+/(.*?)/[\d\.]+/', output)
    if rtt_match:
        latency = f"{rtt_match.group(1)} ms"
        
    return latency, packet_loss

def measure_throughput_tcp(server_ip, client_ip, time=5, netns=None):
    print(f"[*] Measuring TCP Throughput (connecting to {server_ip} from {client_ip})...")
    prefix = f"sudo ip netns exec {netns} " if netns else ""
    cmd = f"{prefix}iperf3 -c {server_ip} -B {client_ip} -t {time} --json"
    output = run_command(cmd)
    
    try:
        data = json.loads(output)
        bps = data['end']['sum_received']['bits_per_second']
        mbps = bps / 1_000_000
        return f"{mbps:.2f} Mbps"
    except (json.JSONDecodeError, KeyError):
        return "N/A (iperf3 failed)"

def measure_jitter_udp(server_ip, client_ip, time=5, bandwidth="10M", netns=None):
    print(f"[*] Measuring UDP Jitter (connecting to {server_ip} from {client_ip})...")
    prefix = f"sudo ip netns exec {netns} " if netns else ""
    cmd = f"{prefix}iperf3 -c {server_ip} -B {client_ip} -u -b {bandwidth} -t {time} --json"
    output = run_command(cmd)
    
    try:
        data = json.loads(output)
        jitter = data['end']['sum']['jitter_ms']
        return f"{jitter:.2f} ms"
    except (json.JSONDecodeError, KeyError):
        return "N/A (iperf3 failed)"

def main():
    parser = argparse.ArgumentParser(description="Measure 5G Performance Metrics over OpenAirInterface")
    parser.add_argument("--ue-interface", default="oaitun_ue1", help="UE network interface (default: oaitun_ue1)")
    parser.add_argument("--server-ip", default="10.45.0.1", help="iperf3 Server IP (default: 10.45.0.1)")
    parser.add_argument("--ping-target", default="8.8.8.8", help="Target IP for ping tests (default: 8.8.8.8)")
    parser.add_argument("--test-duration", type=int, default=5, help="Duration for iperf3 tests in seconds (default: 5)")
    parser.add_argument("--netns", default=None, help="Network namespace for the UE (e.g., ue1)")
    args = parser.parse_args()

    ue_ip = get_interface_ip(args.ue_interface, netns=args.netns)
    if not ue_ip:
        print(f"[!] Error: Could not find IP address for interface {args.ue_interface}.")
        print("[!] Ensure the UE is connected and the interface is UP.")
        sys.exit(1)

    print(f"[*] Found UE IP: {ue_ip} on interface {args.ue_interface}")
    if args.netns:
        print(f"[*] Using network namespace: {args.netns}")
    
    start_iperf3_server(args.server_ip)
    print("-" * 50)
    
    core_latency, core_loss = measure_ping(ue_ip, args.server_ip, count=10, netns=args.netns)
    internet_latency, internet_loss = measure_ping(ue_ip, args.ping_target, count=10, netns=args.netns)
    tcp_throughput = measure_throughput_tcp(args.server_ip, ue_ip, time=args.test_duration, netns=args.netns)
    udp_jitter = measure_jitter_udp(args.server_ip, ue_ip, time=args.test_duration, netns=args.netns)
    
    print("-" * 50)
    print(" " * 12 + "5G PERFORMANCE METRICS SUMMARY")
    print("-" * 50)
    print(f"  5G Core Latency   : {core_latency} (Loss: {core_loss})")
    print(f"  Internet Latency  : {internet_latency} (Loss: {internet_loss})")
    print(f"  TCP Throughput    : {tcp_throughput}")
    print(f"  UDP Jitter        : {udp_jitter}")
    print("-" * 50)

if __name__ == "__main__":
    main()

    