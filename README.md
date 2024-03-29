# CS3205-Labs
Lab assignments done as part of CS3205 - Introduction to Computer Networks <br>
Brief summaries of the assignments are provided below, and comprehensive problem statements are present in [Problem-statements](./Problem-statements/)

## Labs

1. [Echo, Math and simple File servers](./Lab-1/)
    - Echo and Math servers implemented with UDP
    - Simple File server implemented with TCP

2. [Simple DNS server](./Lab-2/)
    - A simple hierarchical DNS system implemented with UDP
    - A client and a set of servers:
        - Local DNS server/Name Resolver
        - Root DNS server
        - TLD DNS servers (2, one each for .com and .edu)
        - Authoritative DNS servers (6, 3 per TLD, with 5 server names per organization)

3. [Cyclic Redundancy Check](./Lab-3/)
    - CRC algorithm implemented to determine checksum
    - Investigated properties regarding independent errors and bursty errors detection

4. [Go-Back-N Protocol](./Lab-4/)
    - GBN protocol for reliable transmission implemented with UDP
    - A sender and a receiver, for generating and sending messages, and acknowledging them

5. [OSPF Routing Algorithm](./Lab-5/)
    - OSPF protocol for topology construction and path-finding implemented with UDP
    - Multiple router instances periodically communicating to determine and advertise link states, and computing paths
    