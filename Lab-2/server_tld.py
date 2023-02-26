#https://pythontic.com/modules/socket/udp-client-server-example
import socket
import sys

# localIP     = "127.0.0.1"
# localPort   = 20001
runningIP = sys.argv[1]
startPort = int(sys.argv[2])
serverNo = int(sys.argv[3])
runningPort = startPort + 54 + serverNo
bufferSize  = 1024

msgFromServer       = "Hello UDP Client"
bytesToSend         = str.encode(msgFromServer)

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
    clientMsg = "Message from Client: {}".format(message)
    clientIP  = "Client IP Address: {}".format(address)
    
    print(clientMsg)
    print(clientIP)

    # Sending a reply to client
    UDPServerSocket.sendto(msgFromServer.encode(), address)
