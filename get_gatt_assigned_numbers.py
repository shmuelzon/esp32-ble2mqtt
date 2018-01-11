#!/usr/bin/env python

import json
import re
try:
  import urllib.request as urlrequest
except ImportError:
  import urllib as urlrequest

GATT_URL = 'https://www.bluetooth.com/specifications/gatt'
SERVICES_URL = GATT_URL + '/services';
CHARACTERISTICS_URL = GATT_URL + '/characteristics'
CHARACTERISTIC_URL = 'https://www.bluetooth.com/api/gatt/XmlFile?xmlFileName='

def build_gatt_regex(gatt_type):
  # Nasty, nasty regex to parse a service/characterisitc row in the HTML.
  return r'<td[^>]*>[^<]*<a[^>]*>([^<]*)</a>[^<]*</td>[^<]*<td[^>]*>' \
    '(org.bluetooth.' + gatt_type + '.[^<]*)</td>[^<]*<td[^>]*>([^<]*)<'

def build_sig_uuid(uuid):
  return '0000{}-0000-1000-8000-00805f9b34fb'.format(format(int(uuid, 16), 'x'))

def get_list(file_name, url, gatt_type):
  data = urlrequest.urlopen(url).read().decode('utf-8')
  regex = build_gatt_regex(gatt_type)
  obj = {}

  for match in re.findall(regex, data):
    # match[0] = Name, match[1] = Type, match[2] = Assigned Number
    obj[build_sig_uuid(match[2])] = match[0].replace(' ', '')

  with open(file_name, 'w') as outfile:
    json.dump(obj, outfile, indent=2, sort_keys=True)

def main():
  get_list('data/services.json', SERVICES_URL, 'service')
  get_list('data/characteristics.json', CHARACTERISTICS_URL, 'characteristic')

if __name__ == '__main__':
  main()
