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
rootServerDict = eval(sys.argv[3])
rootServerAddressPort = (rootServerDict['address'], rootServerDict['port'])
bufferSize  = 1024
logFilePath = Path('logs/NR.output')
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
        UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        UDPClientSocket.sendto(str.encode(message), rootServerAddressPort)
        UDPClientSocket.close()
        break

    clientMsg = "[{}]\tQuery from Client: {}\n".format(dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S"), message)
    clientIP  = "[{}]\tClient IP Address: {}\n".format(dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S"), address)
    
    with open(logFilePath, 'a') as f:
        f.write(clientMsg)
        f.write(clientIP)

    UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

    UDPClientSocket.sendto(str.encode(message), rootServerAddressPort)

    msgFromRootServer = UDPClientSocket.recvfrom(bufferSize)

    TLDServerDict = eval(msgFromRootServer[0].decode())
    TLDServerAddressPort = (TLDServerDict['address'], TLDServerDict['port'])

    if TLDServerAddressPort[0] != ERROR_MSG:
        UDPClientSocket.sendto(str.encode(message), TLDServerAddressPort)

        msgFromTLDServer = UDPClientSocket.recvfrom(bufferSize)

        ADServerDict = eval(msgFromTLDServer[0].decode())
        ADServerAddressPort = (ADServerDict['address'], ADServerDict['port'])

        if ADServerAddressPort[0] != ERROR_MSG:
            UDPClientSocket.sendto(str.encode(message), ADServerAddressPort)

            msgFromADServer = UDPClientSocket.recvfrom(bufferSize)

            msgFromServer = msgFromADServer[0].decode()
        else:
            msgFromServer = ERROR_MSG
    else:
        msgFromServer = ERROR_MSG

    UDPClientSocket.close()

    with open(logFilePath, 'a') as f:
        f.write(f'[{dt.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}]\tResponse sent: {msgFromServer}\n\n')

    # Sending a reply to client
    UDPServerSocket.sendto(msgFromServer.encode(), address)

UDPServerSocket.close()
