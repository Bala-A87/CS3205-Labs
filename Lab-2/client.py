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

addressNR = sys.argv[1]
portNR = int(sys.argv[2])

serverAddressPort   = (addressNR, portNR)
bufferSize          = 1024
ERROR_MSG = 'ERROR'

# Create a UDP socket at client side
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

while True:
    msgFromClient       = input('Enter Server Name: ')
    bytesToSend         = str.encode(msgFromClient)

    # Send to server using created UDP socket
    UDPClientSocket.sendto(bytesToSend, serverAddressPort)

    if msgFromClient == 'bye':
        print('[INFO] All Server Processes are killed. Exiting.')
        break

    # Wait on recvfrom()
    msgFromServer = UDPClientSocket.recvfrom(bufferSize)

    # Wait completed
    msg = msgFromServer[0].decode()

    if msg == ERROR_MSG:
        print('No DNS Record Found')
    else:
        print('DNS Mapping:', msg)

UDPClientSocket.close()