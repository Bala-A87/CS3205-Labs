import sys
import numpy as np

def print_help_quit() -> None:
    """
    Prints command line help message and exits
    """
    print('Usage: python3 gen_inp.py <testfile> <num_inputs> <len_inputs>')
    print('Generates <num_inputs> sample test cases of length <len_inputs>, storing them in <testfile>')
    quit(1)

if len(sys.argv) < 3:
    print_help_quit()

testfile = sys.argv[1]
try:
    num_inputs = int(sys.argv[2])
except:
    print_help_quit()
try:
    len_inputs = int(sys.argv[3])
except:
    print_help_quit()
bits = ['0', '1']

rand_inputs = np.random.choice(bits, (num_inputs, len_inputs), replace=True)
with open(testfile, 'w') as f:
    for rand_input in rand_inputs:
        f.write(''.join(rand_input)+'\n')
