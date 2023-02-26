#https://pythontic.com/modules/socket/udp-client-server-example
import socket
import sys

addressNR = sys.argv[1]
startPort = int(sys.argv[2])
portNR = startPort + 53

# msgFromClient       = "Hello UDP Server"
msgFromClient       = input('Enter message to send: ')
bytesToSend         = str.encode(msgFromClient)
serverAddressPort   = (addressNR, portNR)
bufferSize          = 1024

# Create a UDP socket at client side
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Send to server using created UDP socket
UDPClientSocket.sendto(bytesToSend, serverAddressPort)

#Wait on recvfrom()
msgFromServer = UDPClientSocket.recvfrom(bufferSize)

#Wait completed
msg = "Message from Server: {}".format(msgFromServer[0].decode())

print(msg)

UDPClientSocket.close()