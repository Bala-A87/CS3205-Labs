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

def find_tld(server_name: str) -> str:
    return server_name.split('.')[-1]

runningIP = sys.argv[1]
runningPort = int(sys.argv[2])
TLDServerDict = eval(sys.argv[3])
bufferSize  = 1024
logFilePath = Path('logs/RDS.output')

# msgFromServer       = str(TLDServerDict['com'])
# bytesToSend         = str.encode(msgFromServer)

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
        for tld in TLDServerDict:
            TLDServerAddressPort = (TLDServerDict[tld]['address'], TLDServerDict[tld]['port'])
            UDPClientSocket.sendto(str.encode(message), TLDServerAddressPort)
        UDPClientSocket.close()
        break

    clientMsg = "Query from Client: {}\n".format(message)
    clientIP  = "Client IP Address: {}\n".format(address)
    
    with open(logFilePath, 'a') as f:
        f.write(clientMsg)
        f.write(clientIP)
    # print(clientMsg)
    # print(clientIP)

    tld = find_tld(message)
    msgFromServer = str(TLDServerDict[tld])

    with open(logFilePath, 'a') as f:
        f.write(f'Reponse sent: {msgFromServer}\n')

    # Sending a reply to client
    UDPServerSocket.sendto(msgFromServer.encode(), address)
    # break # deal later

UDPServerSocket.close()