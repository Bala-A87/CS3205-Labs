# Lab 4

## Language coded in 

C++

## Details of submission

### Source code

- [ReceiverGBN](./ReceiverGBN.cpp): The receiver/server
- [SenderGBN](./SenderGBN.cpp): The sender/client

### Running code

The [Makefile](./Makefile) can be used to generate the executables. <br>
`make ReceiverGBN` makes the receiver <br>
`make SenderGBN` makes the sender <br>
`make` or `make all` makes both <br>
`make clean` can be used to clear the executables after execution.

The generated executables can then be run as <br>
<b>Receiver</b> (must be started first): `./ReceiverGBN <-d> <-p PORT> <-n MAX_PKTS> <-e PKT_DROP_PROB>` <br>
`-d` is to be passed to turn on debug mode <br>
`-p PORT` sets the receiver's port to PORT. Default value: 9999 <br>
`-n MAX_PKTS` sets the maximum number of packets acknowledged by receiver to MAX_PKTS. Default value: 500 <br>
`-e PKT_DROP_PROB` sets the packet drop probability to PKT_DROP_PROB. Default value: 1e-4

<b>Sender</b>: `./SenderGBN <-d> <-s ADDRESS> <-p PORT> <-l PKT_LEN> <-n MAX_PKTS> <-r PKT_GEN_RATE> <-w WINDOW_SIZE> <-f GEN_BUF_SIZE>` <br>
`-d` is to be passed to turn on debug mode <br>
`-s ADDRESS` sets the server address to ADDRESS. Default value: 127.0.0.1 <br>
`-p PORT` sets the server port to PORT. Default value: 9999 <br>
`-l PKT_LEN` sets the length of transmitted packets to PKT_LEN. Default value: 256 <br>
`-n MAX_PKTS` sets the maximum number of ACK packets to receive to MAX_PKTS. Default value: 500 <br>
`-r PKT_GEN_RATE` sets the rate at which the packets are generated to PKT_GEN_RATE per second. Default value: 100 <br>
`-w WINDOW_SIZE` sets the transmission window size to WINDOW_SIZE. Default value: 64 <br>
`-f GEN_BUF_SIZE` sets the buffer size where generated packets are stored to GEN_BUF_SIZE. Default value: 200 

