import sys
import socket
from typing import List
import time
import json

id = None               # -i
infile = None           # -f
outfile = None          # -o
hello_interval = 1      # -h
lsa_interval = 5        # -a
spf_interval = 20       # -s

i = 0
while i < len(sys.argv):
    if sys.argv[i] == '-i':
        id = sys.argv[i+1]
        i += 1
    elif sys.argv[i] == '-f':
        infile = sys.argv[i+1]
        i += 1
    elif sys.argv[i] == '-o':
        outfile = sys.argv[i+1]
        i += 1
    elif sys.argv[i] == '-h':
        hello_interval = int(sys.argv[i+1])
        i += 1
    elif sys.argv[i] == '-a':
        lsa_interval = int(sys.argv[i+1])
        i += 1
    elif sys.argv[i] == '-s':
        spf_interval = int(sys.argv[i+1])
        i += 1


def send_hello(neighbors: List[bool], interval: int, socket_comm: socket.socket):
    while True:
        time.sleep(interval)
        for neighbor, is_neighbor in enumerate(neighbors):
            if is_neighbor:
                socket_comm.sendto(str.encode(json.loads(['HELLO', id])), ('127.0.0.1', 10000+neighbor))

def reply_hello(socket_comm: socket.socket, hello_src: int, min_wt: int, max_wt: int):
    while True:
        # send hello reply to the src with a random wt bw min and max
        pass    

    
neighbors = None
min_wts = None
max_wts = None
n, m = None, None

with open(infile, 'r') as f:
    infile_lines = f.read().split('\n')
for line_num, line in enumerate(infile_lines):
    split_line = line.split()
    if line_num == 0:
        n, m = split_line[0], split_line[1]
        neighbors = [False] * n
        min_wts = [None] * n
        max_wts = [None] * n
    else:
        if split_line[0] == id:
            neighbors[split_line[1]] = True
            min_wts[split_line[1]] = split_line[2]
            max_wts[split_line[1]] = split_line[3]
        elif split_line[1] == id:
            neighbors[split_line[0]] = True
            min_wts[split_line[0]] = split_line[2]
            max_wts[split_line[0]] = split_line[3]

port = 10000 + id
socket_comm = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
socket_comm.bind(('127.0.0.1', port))

