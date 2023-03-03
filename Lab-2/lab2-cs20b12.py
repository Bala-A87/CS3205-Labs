# NAME: Balakrishnan A
# Roll Number: CS20B012
# Course: CS3205 Jan. 2023 semester
# Lab number: 2
# Date of submission: 3rd March 2023
# I confirm that the source file is entirely written by me without resorting to any dishonest means.
# Website that I used for basic socket programming code is:
# URL: https://pythontic.com/modules/socket/udp-client-server-example

import os
import sys
from pathlib import Path

LOCALHOST = '127.0.0.1' # the address of the local machine, where the DNS servers are run

if len(sys.argv) < 3:
    print('[ERROR] Missing start port number or input file.')
    print('[INFO] Usage: python3 lab2-cs20b012.py <startportnum> <inputfile>')
    exit(1)

try: # ensure startportnum is an integer
    startPort = int(sys.argv[1])
except:
    print('[ERROR] <startportnum> must be an integer.')
    print('[INFO] Usage: python3 lab2-cs20b012.py <startportnum> <inputfile>')
    exit(1)

mappingFileName = sys.argv[2]
if not Path(mappingFileName).is_file(): # ensure inputfile is present in the directory
    print(f'[ERROR] File {mappingFileName} not found in the directory.')
    exit(1)

logDirPath = Path('logs/')
if not logDirPath.is_dir(): # make directory for logs if not already present
    os.mkdir(logDirPath)

with open(mappingFileName, 'r') as f: # read inputfile line by line and store
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
non_ADS_servers = ['NR', 'RDS', 'TDS_com', 'TDS_edu']
lastMilestone = None
DNSMapping = {} # addresses for each level of the DNS hierarchy
ADSMapping = [{}, {}, {}, {}, {}, {}]
auth_doms = [] # the authoritative domain names for each of the ADSs

for line in mappingFileLines:
    mappingWords = line.split()
    if line in milestones:
        lastMilestone = line
    elif lastMilestone == milestones[0]: 
        DNSMapping[mappingWords[0]] = LOCALHOST # start the server on the local machine
        if mappingWords[0] not in non_ADS_servers:
            auth_doms.append(mappingWords[0])
    elif lastMilestone != milestones[-1]: # get the ADS number and store mapping in the corresponding entry
        ADSMapping[int(lastMilestone[-1])-1][mappingWords[0]] = mappingWords[1]

ADSMapping_tld = {'com': {}, 'edu': {}} # to send required ADS information to the TDSs
for i in range(len(auth_doms)):
    ADSMapping_tld[auth_doms[i].split('.')[-1]][auth_doms[i]] = {'address': DNSMapping[auth_doms[i]], 'port': startPort+57+i}

# the parameters (cmd line args) required to run each server
params = [
    ['server_nr.py', str(DNSMapping['NR']), str(startPort+53), str({'address': DNSMapping['RDS'], 'port': startPort+54})],
    ['server_root.py', str(DNSMapping['RDS']), str(startPort+54), str({
    'com': {'address': DNSMapping['TDS_com'], 'port': startPort+55},
    'edu': {'address': DNSMapping['TDS_edu'], 'port': startPort+56}
    })],
    ['server_tld.py', str(DNSMapping['TDS_com']), str(startPort+55), str(ADSMapping_tld['com'])],
    ['server_tld.py', str(DNSMapping['TDS_edu']), str(startPort+56), str(ADSMapping_tld['edu'])],
    ['server_auth.py', str(DNSMapping[auth_doms[0]]), str(startPort+57), str(ADSMapping[0])],
    ['server_auth.py', str(DNSMapping[auth_doms[1]]), str(startPort+58), str(ADSMapping[1])],
    ['server_auth.py', str(DNSMapping[auth_doms[2]]), str(startPort+59), str(ADSMapping[2])],
    ['server_auth.py', str(DNSMapping[auth_doms[3]]), str(startPort+60), str(ADSMapping[3])],
    ['server_auth.py', str(DNSMapping[auth_doms[4]]), str(startPort+61), str(ADSMapping[4])],
    ['server_auth.py', str(DNSMapping[auth_doms[5]]), str(startPort+62), str(ADSMapping[5])]
]

# the parameters (cmd line args) to run client
client_params = ['client.py', str(DNSMapping['NR']), str(startPort+53)]

print('[INFO] Generating session...')

# generate servers and client
for count in range(10):
    pid = os.fork()
    if pid == 0:
        os.execlp('python3', 'python3', *params[count])
    elif count == 9:
        os.execlp('python3', 'python3', *client_params)
