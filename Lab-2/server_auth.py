# NAME: Balakrishnan A
# Roll Number: CS20B012
# Course: CS3205 Jan. 2023 semester
# Lab number: 2
# Date of submission: TODO
# I confirm that the source file is entirely written by me without resorting to any dishonest means.
# Website that I used for basic socket programming code is:
# URL: https://pythontic.com/modules/socket/udp-client-server-example

import socket
import sys
from pathlib import Path 
import datetime as dt

runningIP = sys.argv[1]
runningPort = int(sys.argv[2])
addressMapping = eval(sys.argv[3])
bufferSize  = 1024
logFilePath = Path('logs/ADS.output')
ERROR_MSG = 'ERROR'

# Create a datagram socket
UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind to address and ip
UDPServerSocket.bind((runningIP, runningPort))

# Listen for incoming datagrams
while (True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    message = bytesAddressPair[0].decode()
    address = bytesAddressPair[1]

    if message == 'bye':
        break

    clientMsg = "[{}]\tQuery from Client: {}\n".format(dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S"), message)
    clientIP  = "[{}]\tClient IP Address: {}\n".format(dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S"), address)
    
    with open(logFilePath, 'a') as f:
        f.write(clientMsg)
        f.write(clientIP)

    if message in addressMapping.keys():
        msgFromServer = addressMapping[message]
    else:
        msgFromServer = ERROR_MSG

    with open(logFilePath, 'a') as f:
        f.write(f'[{dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}]\tResponse sent: {msgFromServer}\n\n')

    # Sending a reply to client
    UDPServerSocket.sendto(msgFromServer.encode(), address)

UDPServerSocket.close()
