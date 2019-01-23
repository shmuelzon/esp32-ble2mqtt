#!/usr/bin/env python

try:
  from http.server import BaseHTTPRequestHandler, HTTPServer
  from socketserver import ThreadingMixIn
except ImportError:
  from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
  from SocketServer import ThreadingMixIn
import argparse
import json
import os
import paho.mqtt.client as mqtt
import shutil
from threading import Thread
import time
import socket
import sys

TIMEOUT = 3
active_connections = 0
last_request_time = time.time()

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
  pass

def OTAServerFactory(args):
  class OTAServer(BaseHTTPRequestHandler, object):
    def __init__(self, *args, **kwargs):
      super(OTAServer, self).__init__(*args, **kwargs)
    def log_message(self, format, *args):
        return
    def do_GET(self):
      global active_connections
      global last_request_time

      active_connections += 1
      last_request_time = time.time()
      print('%s - (%s: %s) GET %s ' % (self.log_date_time_string(),
        self.address_string(), self.headers.get('User-Agent', ''), self.path))

      if self.headers.get('If-None-Match', '').replace('"', '') == args.version:
        self.send_response(304)
        print('%s - (%s: %s) Done: 304 Not Modified' % (
          self.log_date_time_string(), self.address_string(),
          self.headers.get('User-Agent', '')))
        active_connections -= 1
        return

      self.send_response(200)
      self.send_header('Content-Length', os.stat(args.file).st_size)
      self.send_header('ETag', args.version)
      self.end_headers()

      f = open(args.file, 'rb')
      shutil.copyfileobj(f, self.wfile)
      f.close()
      print('%s - (%s: %s) Done: 200 OK' % (
        self.log_date_time_string(), self.address_string(),
        self.headers.get('User-Agent', '')))
      active_connections -= 1

  return OTAServer

def timeout_thread(httpd):
  global active_connections
  global last_request_time

  while active_connections > 0 or time.time() < last_request_time + TIMEOUT:
    time.sleep(0.1)
  httpd.shutdown()

def get_local_ip():
  s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  s.connect(("8.8.8.8", 80))
  ip = s.getsockname()[0]
  s.close()

  return ip

def on_mqtt_connect(client, args, flags, rc):
  client.publish('%s/OTA/%s' % (args.target, args.name),
    'http://%s:%d/' % (get_local_ip(), args.port))
  client.disconnect()

def main():
  parser = argparse.ArgumentParser(description='OTA firmware/config upgrade')
  parser.add_argument('-f', '--file', required=True,
    help='File used for upgrade')
  parser.add_argument('-v', '--version', required=True,
    help='Version of the upgrade file')
  parser.add_argument('-n', '--name', required=True,
    help='The name of image type to publish, e.g. "Firmware", "Config", etc.')
  parser.add_argument('-t', '--target', default='BLE2MQTT',
    help='Host to upgrade. If left empty, will upgrade all hosts')
  parser.add_argument('-p', '--port', type=int, default=8000,
    help='HTTP server port')
  parser.add_argument('--mqtt-broker-server',
    help='MQTT broker server for initiating upgrade procedure. '
      'Default taken from configuration file')
  parser.add_argument('--mqtt-broker-port', type=int,
    help='MQTT broker port for initiating upgrade procedure. '
      'Default taken from configuration file')
  parser.add_argument('--mqtt-broker-username',
    help='MQTT broker username for initiating upgrade procedure. '  
      'Default taken from configuration file')
  parser.add_argument('--mqtt-broker-password',
    help='MQTT broker password for initiating upgrade procedure. '
      'Default taken from configuration file')

  args = parser.parse_args()

  config = json.load(open('data/config.json'))
  if args.mqtt_broker_server is None:
    args.mqtt_broker_server = config['mqtt']['server']['host']
  if args.mqtt_broker_port is None:
    args.mqtt_broker_port = config['mqtt']['server']['port']
  if args.mqtt_broker_username is None and 'username' in config['mqtt']['server']:
    args.mqtt_broker_username = config['mqtt']['server']['username']
  if args.mqtt_broker_password is None and 'password' in config['mqtt']['server']:
    args.mqtt_broker_password = config['mqtt']['server']['password']

  # Connect to MQTT
  mqttc = mqtt.Client(userdata=args)
  mqttc.on_connect = on_mqtt_connect
  if args.mqtt_broker_username is not None:
    mqttc.username_pw_set(args.mqtt_broker_username, args.mqtt_broker_password)
  mqttc.connect(args.mqtt_broker_server, args.mqtt_broker_port)
  mqttc.loop_start()

  # Set up HTTP server
  httpd = ThreadedHTTPServer(("", args.port), OTAServerFactory(args))
  Thread(target=timeout_thread, args=(httpd, )).start()
  print('Listening on port %d' % args.port)
  httpd.serve_forever()
  httpd.server_close()
  print('No new connection requests, shutting down')

if __name__ == '__main__':
  main()
