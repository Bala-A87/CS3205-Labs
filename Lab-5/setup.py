# NAME: Balakrishnan A
# Roll Number: CS20B012
# Course: CS3205 Jan. 2023 semester
# Lab number: 5
# Date of submission: Apr 28, 2023
# I confirm that the source file is entirely written by me without
# resorting to any dishonest means.
# Website(s) that I used for basic socket programming code are:
# URL(s): None
# Website used for Dijkstra algorithm code: 
# https://www.geeksforgeeks.org/python-program-for-dijkstras-shortest-path-algorithm-greedy-algo-7/

import sys, os, signal

# Command line arguments
infile = None           # -f
outfile = None          # -o
hello_interval = 1      # -h
lsa_interval = 5        # -a
spf_interval = 20       # -s
debug = False           # -d

i = 1
while i < len(sys.argv):
    if sys.argv[i] == '-f':
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
    elif sys.argv[i] == '-d':
        debug = True
    i += 1

# Get number of nodes 
with open(infile, 'r') as f:
    for line in f:
        nodes = int(line.split(' ')[0])
        break

# Fork and exec processes to create routers
children = [] # store children's pids for graceful exit later
for node in range(nodes):
    pid = os.fork()
    if pid == 0:
        if debug:
            os.execlp('python3', 'python3', 'ospf.py', '-i', str(node), '-f', infile, '-o', outfile+f'-{node}.txt', '-h', str(hello_interval), '-a', str(lsa_interval), '-s', str(spf_interval), '-d')
        else:
            os.execlp('python3', 'python3', 'ospf.py', '-i', str(node), '-f', infile, '-o', outfile+f'-{node}.txt', '-h', str(hello_interval), '-a', str(lsa_interval), '-s', str(spf_interval))
    else:
        children.append(pid)

kill_signal = input()

if kill_signal == 'stop': # graceful exit
    # Kill all created processes
    for child in children:
        os.kill(child, signal.SIGTERM)
