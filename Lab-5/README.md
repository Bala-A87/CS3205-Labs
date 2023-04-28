# Lab 5

## Language coded in

Python

## Source files

- Main OSPF code: [ospf.py](./ospf.py) 
- Setup file: [setup.py](./setup.py)
- Graph definition: [graph.py](./graph.py)

## Executing code

Each router may be started individually by invoking the command

`python3 ospf.py -i <ID> -f <INFILE> -o <OUTFILE> [-h <HELLO_INTERVAL>] [-a <LSA_INTERVAL>] [-s <SPF_INTERVAL>] [-d]`

where, <br>
ID is the router ID, an integer from 0 to (number of nodes)-1 <br>
INFILE is the path to the input file with a description of the graph
OUTFILE is the path to the file where routing tables are to be logged <br>
HELLO_INTERVAL is the time interval (in seconds, integer) between HELLO packets sent by this router (default: 1) <br>
LSA_INTERVAL is the time interval (in seconds, integer) between LSA packets sent by this router (default: 5) <br>
SPF_INTERVAL is the time interval (in seconds, integer) between consecutive shortest paths computation by this router (default: 20) <br>
-d is set to turn on debug mode, where transmissions and receipts of different packets by this router are logged

Multiple routers can be started using the setup file for a generic output file name and common interval values by invoking

`python3 setup.py -f <INFILE> -o <OUTFILE> [-h <HELLO_INTERVAL>] [-a <LSA_INTERVAL>] [-s <SPF_INTERVAL>] [-d]`

with the same meanings except that these values are now common across all routers. OUTFILE for each router is set as OUTFILE-ID.txt, so no file extensions in OUTFILE for this case is better. In order to perform a graceful exit, enter `stop` when you wish to terminate the program.

The folders [inputs](./inputs/) and [outputs](./outputs/) are provided to house sample inputs and corresponding outputs. 4 sample cases are provided in inputs, with two of them ([sample_2](./inputs/sample_2.txt) and [sample_3](./inputs/sample_3.txt)) being static graphs with min link weight = max link weight. The outputs for [sample_3](./inputs/sample_3.txt) and [sample_4](./inputs/sample_4.txt) are present in the directory.