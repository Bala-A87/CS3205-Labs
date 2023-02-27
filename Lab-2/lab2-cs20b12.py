# NAME: Balakrishnan A
# Roll Number: CS20B012
# Course: CS3205 Jan. 2023 semester
# Lab number: 2
# Date of submission: TODO
# I confirm that the source file is entirely written by me without resorting to any dishonest means.
# Website that I used for basic socket programming code is:
# URL: https://pythontic.com/modules/socket/udp-client-server-example

import os
import sys
from pathlib import Path

if len(sys.argv) < 3:
    print('[ERROR] Missing start port number or input file.')
    print('[INFO] Usage: python3 lab2-cs20b012.py <startportnum> <inputfile>')
    exit(1)

startPort = int(sys.argv[1])
mappingFileName = sys.argv[2]
logDirPath = Path('logs/')
if not logDirPath.is_dir():
    os.mkdir(logDirPath)

with open(mappingFileName, 'r') as f:
    mappingFileLines = f.read().split('\n')

milestones = [
    'BEGIN_DATA',
    'List_of_ADS1',
    'List_of_ADS2',
    'List_of_ADS3',
    'List_of_ADS4',
    'List_of_ADS5',
    'List_of_ADS6',
    'END_DATA'
]
lastMilestone = None
DNSMapping = {}
ADS1Mapping = {}
ADS2Mapping = {}
ADS3Mapping = {}
ADS4Mapping = {}
ADS5Mapping = {}
ADS6Mapping = {}

for line in mappingFileLines:
    mappingWords = line.split()
    if line in milestones:
        lastMilestone = line
    elif lastMilestone == milestones[0]:
        DNSMapping[mappingWords[0]] = mappingWords[1]
    elif lastMilestone == milestones[1]:
        ADS1Mapping[mappingWords[0]] = mappingWords[1]
    elif lastMilestone == milestones[2]:
        ADS2Mapping[mappingWords[0]] = mappingWords[1]
    elif lastMilestone == milestones[3]:
        ADS3Mapping[mappingWords[0]] = mappingWords[1]
    elif lastMilestone == milestones[4]:
        ADS4Mapping[mappingWords[0]] = mappingWords[1]
    elif lastMilestone == milestones[5]:
        ADS5Mapping[mappingWords[0]] = mappingWords[1]
    elif lastMilestone == milestones[6]:
        ADS6Mapping[mappingWords[0]] = mappingWords[1]

ADSMapping_com = {}
ADSMapping_edu = {}

for ADS_no, mappings in enumerate([ADS1Mapping, ADS2Mapping, ADS3Mapping, ADS4Mapping, ADS5Mapping, ADS6Mapping]):
    ex_dom = list(mappings.keys())[0].split('.')
    tld = ex_dom[-1]
    auth_dom = ex_dom[-2] + '.' + ex_dom[-1]
    if tld == 'com':
        ADSMapping_com[auth_dom] = {'address': DNSMapping['ADS'+str(ADS_no+1)], 'port': startPort+57+ADS_no}
    else:
        ADSMapping_edu[auth_dom] = {'address': DNSMapping['ADS'+str(ADS_no+1)], 'port': startPort+57+ADS_no}

params = [
    ['server_nr.py', str(DNSMapping['NR']), str(startPort+53), str({'address': DNSMapping['RDS'], 'port': startPort+54})],
    ['server_root.py', str(DNSMapping['RDS']), str(startPort+54), str({
    'com': {'address': DNSMapping['TDS_com'], 'port': startPort+55},
    'edu': {'address': DNSMapping['TDS_edu'], 'port': startPort+56}
    })],
    ['server_tld.py', str(DNSMapping['TDS_com']), str(startPort+55), str(ADSMapping_com)],
    ['server_tld.py', str(DNSMapping['TDS_edu']), str(startPort+56), str(ADSMapping_edu)],
    ['server_auth.py', str(DNSMapping['ADS1']), str(startPort+57), str(ADS1Mapping)],
    ['server_auth.py', str(DNSMapping['ADS2']), str(startPort+58), str(ADS2Mapping)],
    ['server_auth.py', str(DNSMapping['ADS3']), str(startPort+59), str(ADS3Mapping)],
    ['server_auth.py', str(DNSMapping['ADS4']), str(startPort+60), str(ADS4Mapping)],
    ['server_auth.py', str(DNSMapping['ADS5']), str(startPort+61), str(ADS5Mapping)],
    ['server_auth.py', str(DNSMapping['ADS6']), str(startPort+62), str(ADS6Mapping)]
]

client_params = ['client.py', str(DNSMapping['NR']), str(startPort+53)]

print('[INFO] Generating session...')

for count in range(10):
    pid = os.fork()
    if pid == 0:
        os.execlp('python3', 'python3', *params[count])
    elif count == 9:
        os.execlp('python3', 'python3', *client_params)