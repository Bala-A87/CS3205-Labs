#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void copy_int_arr(int *old_arr, int *new_arr, int len, int old_start = 0, int new_start = 0) {
    for(int i = 0; i < len; i++)
        new_arr[new_start+i] = old_arr[old_start+i];
}

class Packet {
public:
    int seq_no_len, msg_len;
    int *seq_no, *msg;

    Packet() {
        seq_no_len = msg_len = 0;
        seq_no = msg = NULL;
    }

    Packet(int seq_no_len, int msg_len, int *seq_no, int *msg = NULL) {
        this->seq_no_len = seq_no_len;
        this->msg_len = msg_len;
        this->seq_no = new int[seq_no_len];
        if(msg_len != 0) {
            this->msg = new int[msg_len];
            if(msg != NULL)
                copy_int_arr(msg, this->msg, msg_len);
        }
        copy_int_arr(seq_no, this->seq_no, seq_no_len);
    }

    Packet(const Packet &packet) {
        seq_no_len = packet.seq_no_len;
        msg_len = packet.msg_len;
        seq_no = new int[seq_no_len];
        msg = new int[msg_len];
        copy_int_arr(packet.seq_no, seq_no, seq_no_len);
        copy_int_arr(packet.msg, msg, msg_len);
    }
};