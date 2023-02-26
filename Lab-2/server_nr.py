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
import os
from pathlib import Path

runningIP = sys.argv[1]
runningPort = int(sys.argv[2])
rootServerDict = eval(sys.argv[3])
rootServerAddressPort = (rootServerDict['address'], rootServerDict['port'])
bufferSize  = 1024
logDirPath = Path('logs/')
if not logDirPath.is_dir(): # offload to forking script (main file)
    os.mkdir(logDirPath)
logFilePath = Path('logs/NR.output')

# Create a datagram socket
UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind to address and ip
UDPServerSocket.bind((runningIP, runningPort))

print("UDP server up and listening")

# Listen for incoming datagrams
while (True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    message = bytesAddressPair[0].decode()
    address = bytesAddressPair[1]

    if message == 'bye':
        UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        UDPClientSocket.sendto(str.encode(message), rootServerAddressPort)
        UDPClientSocket.close()
        break

    clientMsg = "Query from Client: {}\n".format(message) # maybe add time to logs?
    clientIP  = "Client IP Address: {}\n".format(address)
    
    with open(logFilePath, 'a') as f:
        f.write(clientMsg)
        f.write(clientIP)

    UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

    UDPClientSocket.sendto(str.encode(message), rootServerAddressPort)

    msgFromRootServer = UDPClientSocket.recvfrom(bufferSize)

    TLDServerDict = eval(msgFromRootServer[0].decode())
    TLDServerAddressPort = (TLDServerDict['address'], TLDServerDict['port'])

    UDPClientSocket.sendto(str.encode(message), TLDServerAddressPort)

    msgFromTLDServer = UDPClientSocket.recvfrom(bufferSize)

    ADServerDict = eval(msgFromTLDServer[0].decode())
    ADServerAddressPort = (ADServerDict['address'], ADServerDict['port'])

    UDPClientSocket.sendto(str.encode(message), ADServerAddressPort)

    msgFromADServer = UDPClientSocket.recvfrom(bufferSize)

    msgFromServer = msgFromADServer[0].decode()

    UDPClientSocket.close()

    with open(logFilePath, 'a') as f:
        f.write(f'Response sent: {msgFromServer}\n')

    # Sending a reply to client
    UDPServerSocket.sendto(msgFromServer.encode(), address)

    # break # deal later

UDPServerSocket.close()
