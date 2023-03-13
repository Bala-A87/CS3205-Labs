import sys # for argument parsing
import numpy as np # for random number generation
from typing import List

infile = sys.argv[1] # file with input test cases
outfile = sys.argv[2] # file where output is printed

with open(infile, 'r') as f:
    entries = f.read().split('\n') # find all testcases
    if '' in entries: # ignore empty lines if present at end of all testcases
        entries.remove('')

G = '100000111' # CCITT CRC-8 standard
LEN_G = len(G)
LEN_M = len(entries[0])
odd_range = list(range(3, (LEN_M+LEN_G)//5, 2))
# Range of number of errors to make
NO_ERROR_REM = list('0'*(LEN_G-1)) # no error detected remainder
NUM_RAND_ERRORS = 10
NUM_BURSTY_ERRORS = 5
BURST_WINDOW_START = 100
BURST_WINDOW_END = 111
BURST_LENGTH = 6

def find_sub_mod2(window: List[str]) -> List[str]:
    """
    Computes subtraction modulo 2 of the input string and G

    Arg: window (list[str]): Input string, of length same as G
    Returns: sub(window, G) (list[str]): Output, of length len(G)-1
    """
    if window[0] == '0': # nothing to subtract, just return all except first bit
        return window[1:]
    else: # xor 
        result = list('1'*(LEN_G-1))
        for i in range(1, LEN_G):
            if G[i] == window[i]:
                result[i-1] = '0'
        return result

def compute_checksum(msg: List[str]) -> List[str]:
    """
    Computes the CRC checksum of msg wrt generator G

    Arg: msg (list[str]: The message before checksum appending)
    Returns: checksum (list[str]): The checksum to append before message transmission
    """
    padded_msg = msg + list('0'*(LEN_G-1)) # pad with zeros for final borrows
    curr_window = padded_msg[:LEN_G] # maintain sliding window of dividend
    next_pos = LEN_G
    while next_pos < len(padded_msg):
        result = find_sub_mod2(curr_window) # current remainder
        curr_window = result + [padded_msg[next_pos]] # next dividend
        next_pos += 1
    return find_sub_mod2(curr_window) # last remainder not calculated, do it here

def list_to_str(tx_list: List[str]) -> str:
    """
    Converts a list of characters to a string

    Arg: tx_list (list[str]): The list of characters to convert
    Returns: tx_str (str): The string form of tx_list
    """
    return ''.join(tx_list)

with open(outfile, 'w') as f:
    for entry in entries:
        entry_list = list(entry) # convert string to list for operations
        entry_w_crc = entry_list + compute_checksum(entry_list) # transmitted message, sender perspective
        num_errors = np.random.choice(odd_range, NUM_RAND_ERRORS, replace=True) # number of random errors to generate
        for count in num_errors:
            error_pos = np.random.choice(range(LEN_G+LEN_M-1), size=count, replace=False) # the error bit positions
            corrupted_string = entry_w_crc.copy()
            for pos in error_pos: # transmitted string corrupted with errors
                corrupted_string[pos] = '1' if corrupted_string[pos]=='0' else '0'
            f.write(f'Original String: {entry}\n')
            f.write(f'Original String with CRC: {list_to_str(entry_w_crc)}\n')
            f.write(f'Corrupted String: {list_to_str(corrupted_string)}\n')
            f.write(f'Number of Errors Introduced: {count}\n')
            remainder = compute_checksum(corrupted_string) # check for error, receiver perspective
            error_detected = not(remainder == NO_ERROR_REM)
            f.write(f'CRC Check: {"Failed" if error_detected else "Passed"}\n\n')
        bursty_starts = np.random.uniform(BURST_WINDOW_START, BURST_WINDOW_END, NUM_BURSTY_ERRORS).astype(int)
        # Positions of bursty error starting
        for start in bursty_starts:
            corrupted_string = entry_w_crc.copy()
            for pos in range(start, start+BURST_LENGTH):
                corrupted_string[pos] = '1' if corrupted_string[pos]=='0' else '0'
            f.write(f'Original String: {entry}\n')
            f.write(f'Original String with CRC: {list_to_str(entry_w_crc)}\n')
            f.write(f'Corrupted String: {list_to_str(corrupted_string)}\n')
            f.write(f'Start of Bursty Error: {start}\n')
            remainder = compute_checksum(corrupted_string)
            error_detected = not(remainder == NO_ERROR_REM)
            f.write(f'CRC Check: {"Failed" if error_detected else "Passed"}\n\n')
        f.write('*'*100+'\n\n')

