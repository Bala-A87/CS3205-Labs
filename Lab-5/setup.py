import sys, os, signal

nodes = None            # -n
infile = None           # -f
outfile = None          # -o
hello_interval = 1      # -h
lsa_interval = 5        # -a
spf_interval = 20       # -s
debug = False           # -d

i = 1
while i < len(sys.argv):
    if sys.argv[i] == '-n':
        nodes = int(sys.argv[i+1])
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
    elif sys.argv[i] == '-d':
        debug = True
    i += 1

children = []
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

if kill_signal == 'stop':
    for child in children:
        os.kill(child, signal.SIGTERM)
