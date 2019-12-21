#!/usr/bin/env python

from __future__ import print_function
from builtins import str
import argparse
import datetime
import ipaddress
import json
import os
import socket
import struct

dns_cache = dict()

def get_hostname(addr):
  ip = addr[0]
  try:
    if dns_cache.get(ip) == None:
      name, alias, addresslist = socket.gethostbyaddr(ip)
      dns_cache[ip] = name
    return dns_cache[ip]
  except socket.herror:
    return ip

def log_listener(args):
  ip = ipaddress.ip_address(str(socket.gethostbyname(args.host)))

  # Set up logging server
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  if ip.is_multicast:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    mreq = struct.pack("4sl", socket.inet_aton(str(ip)), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
  sock.bind(('', args.port))

  print('Listening on port %d' % args.port)
  while True:
    try:
      data, addr = sock.recvfrom(2048)
      print('%s (%s): %s' % (datetime.datetime.now(), get_hostname(addr),
        data.decode('utf-8', 'ignore')), end='')
    except KeyboardInterrupt:
      break;

def main():
  parser = argparse.ArgumentParser(description='Remote logging server')
  parser.add_argument('--host', help='Host or IP address to listen on. '
    'Default take from configuration file')
  parser.add_argument('--port', type=int, help='UDP port to listen on. '
    'Default take from configuration file')
  args = parser.parse_args()

  config = json.load(open('data/config.json'))
  try:
    if args.host is None:
      args.host = config['log']['host']
    if args.port is None:
      args.port = config['log']['port']
  except KeyError:
    print('Logging seems to be disabled in the configuration file')
    return

  log_listener(args)

if __name__ == '__main__':
  main()
