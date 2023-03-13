import sys

infile = sys.argv[1]
outfile = sys.argv[2]

with open(infile, 'r') as f:
    entries = f.read().split('\n')

G = '100000111'
LEN_G = 9
LEN_M = len(entries[0])

def find_xor(window: str) -> str:
    if window[0] == 0:
        return window[1:]
    else:
        result = '1'*(LEN_G-1)
        for i in range(1, LEN_G):
            if G[i] == window[i]:
                result[i-1] = '0'
        return result

def compute_checksum(msg: str) -> str:
    curr_window = msg[:LEN_G]
    next_pos = LEN_G
    while next_pos < LEN_M:
        result = find_xor(curr_window)
        curr_window = result + msg[next_pos]
        next_pos += 1
    return find_xor(curr_window)

