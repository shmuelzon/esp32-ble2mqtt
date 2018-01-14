#!/usr/bin/env python

import argparse
import json
import re
from multiprocessing.dummy import Pool as ThreadPool
try:
  import urllib.request as urlrequest
except ImportError:
  import urllib as urlrequest

GATT_URL = 'https://www.bluetooth.com/specifications/gatt'
SERVICES_URL = GATT_URL + '/services';
CHARACTERISTICS_URL = GATT_URL + '/characteristics'
CHARACTERISTIC_URL = 'https://www.bluetooth.com/api/gatt/XmlFile?xmlFileName='

services = {}
characteristics = {}
characteristics_types = {}

def build_gatt_regex(gatt_type):
  # Nasty, nasty regex to parse a service/characterisitc row in the HTML.
  return r'<td[^>]*>[^<]*<a[^>]*>([^<]*)</a>[^<]*</td>[^<]*<td[^>]*>' \
    '(org.bluetooth.' + gatt_type + '.[^<]*)</td>[^<]*<td[^>]*>([^<]*)<'

def build_sig_uuid(uuid):
  return '%08x-0000-1000-8000-00805f9b34fb' % (int(uuid, 16))

def parse_services(data):
  regex = build_gatt_regex('service')

  # Parse all services
  for service in re.findall(regex, data):
    # service[0] = Name, service[1] = Type, service[2] = Assigned Number
    uuid = build_sig_uuid(service[2])
    services[uuid] = {
      'name': service[0].replace(' ', ''),
    }

def get_characteristics_types(uuid):
  url = CHARACTERISTIC_URL + characteristics[uuid]['org'] + '.xml'
  data = urlrequest.urlopen(url).read().decode('utf-8')
  print('Downloaded ' + characteristics[uuid]['org'])

  for char_type in re.findall(r'<Format>([^<]*)<\/Format>', data):
    enum_type = 'CHAR_TYPE_' + char_type.replace('-', '_').upper()
    characteristics[uuid]['types'].append(enum_type)
    characteristics_types[enum_type] = True

def parse_characteristics(data):
  regex = build_gatt_regex('characteristic')
  threads = []

  # Parse all characteristics
  for char in re.findall(regex, data):
    # char[0] = Name, char[1] = Type, char[2] = Assigned Number
    uuid = build_sig_uuid(char[2])
    characteristics[uuid] = {
      'name': char[0].replace(' ', ''),
      'org': char[1],
      'types': []
    }

  # Download all characteristic definitions
  pool = ThreadPool(10)
  pool.map(get_characteristics_types, characteristics.keys())
  pool.close()
  pool.join()

def get_list(url, parser):
  data = urlrequest.urlopen(url).read().decode('utf-8')
  parser(data)

def write_h(filename):
  with open(filename, 'w') as outfile:
    outfile.write(
      '#ifndef GATT_H\n' \
      '#define GATT_H\n' \
      '\n' \
      '#include "ble.h"\n' \
      '\n' \
      'typedef enum {\n' \
      '    %s\n'
      '} characteristic_type_t;\n' \
      '\n' \
      'typedef struct {\n' \
      '    ble_uuid_t uuid;\n' \
      '    char *name;\n' \
      '    characteristic_type_t *types;\n' \
      '} characteristic_desc_t;\n' \
      '\n' \
      'typedef struct {\n' \
      '    ble_uuid_t uuid;\n' \
      '    char *name;\n' \
      '} service_desc_t;\n' \
      '\n' \
      'extern service_desc_t services[];\n' \
      'extern characteristic_desc_t characteristics[];\n' \
      '\n' \
      '#endif' % (',\n    '.join(characteristics_types.keys()))
    )
    outfile.close()

def write_c(filename):
  with open(filename, 'w') as outfile:
    outfile.write(
      '#include "gatt.h"\n' \
      '#include <stdlib.h>\n' \
      '\n' \
      'service_desc_t services[] = {\n');

    # Write services definitions
    for uuid, service in sorted(services.iteritems()):
      outfile.write(
        '    {\n' \
        '        .uuid = { %s },\n' \
        '        .name = "%s",\n' \
        '    },\n' % (', '.join('0x{0}'.format(t) for t in
        re.findall('..', uuid.replace('-', ''))[::-1]), service['name']));

    # Terminate services list
    outfile.write(
      '    {}\n' \
      '};\n' \
      '\n');

    # Write characteristic types for each characteristic
    for uuid, char in sorted(characteristics.iteritems()):
      outfile.write('static characteristic_type_t types_%s[] = { %s-1 };\n' %
        (uuid.replace('-', '_'),
        ''.join('{0}, '.format(t) for t in char['types'])));

    # Write characteristics definitions
    outfile.write('\ncharacteristic_desc_t characteristics[] = {\n' );
    for uuid, char in sorted(characteristics.iteritems()):
      outfile.write(
        '    {\n' \
        '        .uuid = { %s },\n' \
        '        .name = "%s",\n' \
        '        .types = types_%s,\n' \
        '    },\n' %
        (', '.join('0x{0}'.format(t) for t in
        re.findall('..', uuid.replace('-', ''))[::-1]), char['name'],
        uuid.replace('-', '_')));

    # Terminate characteristics list
    outfile.write(
      '    {}\n' \
      '};');
    outfile.close

def main():
  parser = argparse.ArgumentParser(description='Obtain Bluetooth SIG GATT IDs')
  parser.add_argument('-C', metavar='gatt.c', help='Output C file')
  parser.add_argument('-H', metavar='gatt.h', help='Output H file')
  args = parser.parse_args()

  print('Getting list of services')
  get_list(SERVICES_URL, parse_services)
  print('Getting list of characteristics')
  get_list(CHARACTERISTICS_URL, parse_characteristics)

  print('Generating source code')
  write_h(args.H)
  write_c(args.C)

if __name__ == '__main__':
  main()
