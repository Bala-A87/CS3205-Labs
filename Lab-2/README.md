# Lab 2

## Language coded in

Python

## Source files
- Client: client.py <br>
- Local DNS server: server_nr.py <br>
- RDS: server_root.py <br>
- TDS: server_tld.py <br>
- ADS: server_auth.py <br>
- Session generator: lab2-cs20b012.py <br>

## Executing code

`python3 lab2-cs20b012.py <startportnum> <inputfile>` <br>
where \<startportnum\> is the number K (>1024) to use to assign port numbers to the respective servers, and \<inputfile\> is the file containing the name-address mapping as specified in the problem statement. Makefile is not required as no compilation is needed.

Running the above will create processes for all the (10) servers, and will proceed to run the client, allowing the user to provide server names and receive the corresponding IP addresses (if valid mappings are present in \<inputfile\>) or a message notifying the user that no corresponding DNS record exists and could not be resolved to an IP address.

Logs for the servers can be found in the [logs](logs/) directory, with one log/output file for each level of the DNS hierarchy, making the communication between the client and the local DNS server as well as further communications between servers transparent.

An example \<inputfile\>, [mapping-file](mapping-file), is provided in the directory, and an example \<startportnum\> is 9999.