import sys
import socket
from typing import List
import time
import json
from numpy.random import default_rng
from graph import Graph
from threading import Thread, Lock
from timeit import default_timer as timer

id = None               # -i
infile = None           # -f
outfile = None          # -o
hello_interval = 1      # -h
lsa_interval = 5        # -a
spf_interval = 20       # -s
debug = False           # -d

i = 1
while i < len(sys.argv):
    if sys.argv[i] == '-i':
        id = int(sys.argv[i+1])
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

BUFSIZE = 1024
graph_mutex = Lock()

def add_log(log_str: str, file: str = outfile):
    with open(file, 'a') as f:
        f.write(log_str + '\n')


def send_hello(neighbors: List[bool], interval: int, socket_comm: socket.socket):
    while True:
        time.sleep(interval)
        for neighbor, is_neighbor in enumerate(neighbors):
            if is_neighbor:
                if debug:
                    add_log(f'Sending HELLO packet to {neighbor}')
                socket_comm.sendto(str.encode(json.dumps(['HELLO', id])), ('127.0.0.1', 10000+neighbor))

def listen_msgs(socket_comm: socket.socket, min_wts: List[int], max_wts: List[int], local_graph: Graph, neighbors: List[bool], last_seq_no_recvd: List[int]):
    while True:
        msg_addr = socket_comm.recvfrom(BUFSIZE) 
        msg = json.loads(msg_addr[0].decode())
        nbr_addr = msg_addr[1]

        if msg[0] == 'HELLO':
            nbr_id = msg[1]
            if debug:
                add_log(f'Received HELLO from {nbr_id}')
            link_wt = int(default_rng().integers(min_wts[nbr_id], max_wts[nbr_id]+1))
            helloreply = ['HELLOREPLY', id, nbr_id, link_wt]
            socket_comm.sendto(str.encode(json.dumps(helloreply)), nbr_addr)

        elif msg[0] == 'HELLOREPLY':
            nbr_id = msg[1]
            link_wt = msg[3]
            if debug:
                add_log(f'Received HELLOREPLY from {nbr_id}, link weight = {link_wt}')
            graph_mutex.acquire()
            local_graph.addEdge(id, nbr_id, link_wt)
            graph_mutex.release()
        
        elif msg[0] == 'LSA':
            src_id = msg[1]
            src_seq_no = msg[2]
            # verify if greater than prev seq num from this src
            if src_seq_no > last_seq_no_recvd[src_id]:
                if debug:
                    add_log(f'Received LSA packet #{src_seq_no} from {src_id}')
                last_seq_no_recvd[src_id] = src_seq_no
                entries = msg[3]
                graph_mutex.acquire()
                for entry in range(entries):
                    scr_nbr_id = msg[2*entry + 4]
                    src_nbr_wt = msg[2*entry + 5]
                    local_graph.addEdge(src_id, scr_nbr_id, src_nbr_wt)
                graph_mutex.release()

                for nbr_id, is_nbr in enumerate(neighbors):
                    if nbr_id == src_id:
                        continue
                    if is_nbr:
                        if debug:
                            add_log(f'Forwarding LSA packet #{src_seq_no} from {src_id} to {nbr_id}')
                        socket_comm.sendto(str.encode(json.dumps(msg)), ('127.0.0.1', 10000+nbr_id))

def send_lsa(neighbors: List[bool], local_graph: Graph, interval: int):
    last_seq_no = 0
    lsa_pkt_base = ['LSA', id]
    num_entries = 0
    for nbr_id, is_nbr in enumerate(neighbors):
        if is_nbr:
            num_entries += 1
    
    while True:
        time.sleep(interval)
        last_seq_no += 1
        lsa_pkt = lsa_pkt_base + [last_seq_no, num_entries]
        graph_mutex.acquire()
        for nbr_id, is_nbr in enumerate(neighbors):
            if is_nbr:
                nbr_link_wt = local_graph.getEdgeWeight(id, nbr_id)
                lsa_pkt += [nbr_id, nbr_link_wt]
        graph_mutex.release()

        for nbr_id, is_nbr in enumerate(neighbors):
            if is_nbr:
                if debug:
                    add_log(f'Sending LSA packet #{last_seq_no} to {nbr_id}')
                socket_comm.sendto(str.encode(json.dumps(lsa_pkt)), ('127.0.0.1', 10000+nbr_id))

def compute_shortest_paths(graph: Graph, interval: int, start_time: float):
    while True:
        time.sleep(interval)
        graph_mutex.acquire()
        dists, hops = local_graph.dijkstra(id)
        graph_mutex.release()
        curr_time = int(timer() - start_time)
        add_log(f'Routing table for Node No. {id} at Time {curr_time}')
        to_log = [None] * graph.V
        dest_length = len('Destination')
        path_length = len('Path')
        to_log[0] = ['Destination', 'Path', 'Cost']
        itr = 1
        for dest in range(graph.V):
            if dest == id:
                continue
            rev_path = []
            next_hop = dest
            while next_hop is not None:
                rev_path += [str(next_hop)]
                next_hop = hops[next_hop]
            path = '-'.join(rev_path[::-1])
            dest_length = max(dest_length, len(str(dest)))
            path_length = max(path_length, len(path))
            to_log[itr] = [str(dest), path, str(dists[dest])]
            itr += 1
        
        for log_str in to_log:
            add_log(log_str[0] + (dest_length+4-len(log_str[0]))*' ' + log_str[1] + (path_length+4-len(log_str[1]))*' ' + log_str[2])
        add_log('')


    
neighbors = None
min_wts = None
max_wts = None
n, m = None, None

with open(infile, 'r') as f:
    infile_lines = f.read().split('\n')
for line_num, line in enumerate(infile_lines):
    split_line = line.split()
    if split_line == []:
        continue
    if len(split_line) == 2:
        n, m = int(split_line[0]), int(split_line[1])
        neighbors = [False] * n
        min_wts = [None] * n
        max_wts = [None] * n
    else:
        if int(split_line[0]) == id:
            neighbors[int(split_line[1])] = True
            min_wts[int(split_line[1])] = int(split_line[2])
            max_wts[int(split_line[1])] = int(split_line[3])
        elif int(split_line[1]) == id:
            neighbors[int(split_line[0])] = True
            min_wts[int(split_line[0])] = int(split_line[2])
            max_wts[int(split_line[0])] = int(split_line[3])

local_graph = Graph(n)

port = 10000 + id
socket_comm = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
socket_comm.bind(('127.0.0.1', port))
if debug:
    add_log(f'Listening on port {port}')

start_time = timer()

hello_thread = Thread(target=send_hello, args=[neighbors, hello_interval, socket_comm])
listen_thread = Thread(target=listen_msgs, args=[socket_comm, min_wts, max_wts, local_graph, neighbors, [0]*n])
lsa_thread = Thread(target=send_lsa, args=[neighbors, local_graph, lsa_interval])
spf_thread = Thread(target=compute_shortest_paths, args=[local_graph, spf_interval, start_time])

hello_thread.start()
listen_thread.start()
lsa_thread.start()
spf_thread.start()

hello_thread.join()
listen_thread.join()
lsa_thread.join()
spf_thread.join()
