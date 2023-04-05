# CS3205 Lab-4: GBN

With increasing drop probability, the retransmission ratio increases for the sender. Typically it can be expected to be close to 1+x, where x is the drop probability. Since the values $10^{-4}, 10^{-8}$ are very small, it does not reflect well in the ratio, but for values such as 0.1 it is visible clearly.

Sometimes the packets take a long time to reach the sender, leading to timeouts. Due to this, we restrict ourselves to fewer packets to get the complete result. This is especially observed only for low error rates, as for high error rates, packet drops due to receiever-side errors begin to prevail. However, restricting to fewer packets causes retransmission ratio to simply be 1.

The round-trip time seems to increase with the packet length (although not to a very significant extent), due to increase in duration of sending packet as the packets are larger.

## Tables

### Packet drop probability = $10^{-4}$ 

| Packet length | Retransmission ratio | Average RTT ($\mu s$)|
| :-----------: | :------------------: | :-------------------:|
| 128 B         | 1                    | 109                  |
| 1024 B        | 1                    | 123                  |

### Packet drop probability = $10^{-8}$

| Packet length | Retransmission ratio | Average RTT ($\mu s$)|
| :-----------: | :------------------: | :-------------------:|
| 128 B         | 1                    | 118                  |
| 1024 B        | 1                    | 137                  |
