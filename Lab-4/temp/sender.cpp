// Client side implementation of UDP client-server model
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include "packet.cpp"

#define PORT	 8080
#define MAXLINE 1024

void increment_seq_no(int *seq_no, int seq_no_len) {
	int carry = 1;
	for(int i = seq_no_len-1; i >= 0; i--) {
		seq_no[i] = seq_no[i] + carry;
		carry = 0;
		if(seq_no[i] >= 256) {
			seq_no[i] %= 256;
			carry = 1;
		}
	} 
}

bool compare_seq_no(int *ack_pkt, int *data_pkt, int seq_no_len) {
	for(int i = 0; i < seq_no_len; i++)
		if(ack_pkt[i] != data_pkt[i])
			return false;
	return true;
}

bool seq_no_geq(int *pkt1, int *pkt2, int seq_no_len) {
	for(int i = 0; i < seq_no_len; i++)
		if(pkt1[i] == 256 || pkt2[i] == 256)
			return false;
		else if(pkt1[i] > pkt2[i])
			return true;
		else if(pkt1[i] < pkt2[i])
			return false;
	return true;
}

bool pkt_in_window(int *ack_pkt, int *window_start, int window_size, int seq_no_len) {
	int *window_end = new int[seq_no_len];
	int carry = window_size-1;
	for(int i = seq_no_len-1; i >= 0; i--) {
		window_end[i] = window_start[i] + carry;
		carry = 0;
		if(window_end[i] >= 256) {
			carry = window_end[i] / 256;
			window_end[i] %= 256;
		}
	}
	bool ans = false;
	if(seq_no_geq(ack_pkt, window_start, seq_no_len) && seq_no_geq(window_end, ack_pkt, seq_no_len))
		ans = true;
	else if(seq_no_geq(ack_pkt, window_start, seq_no_len) && (!seq_no_geq(window_end, ack_pkt, seq_no_len) && carry > 0))
		ans = true;
	else if((!seq_no_geq(ack_pkt, window_start, seq_no_len) && carry > 0) && seq_no_geq(window_end, ack_pkt, seq_no_len))
		ans = true;
	free(window_end);
	return ans;
}

void gen_packets(std::queue<int *> &packets, int num, int pkt_gen_rate, int pkt_len, int seq_field_len, int max_buffer_size, int *curr_seq_no) {
	int i = 0;
	while(true) {
		if(i >= num)
			break;
		int *msg = new int[pkt_len];
		for(int j = 0; j < seq_field_len; j++)
			msg[j] = curr_seq_no[j];
		msg[seq_field_len] = 256;
		for(int j = seq_field_len+1; j < pkt_len; j++)
			msg[j] = rand()%256;
		while(packets.size() >= max_buffer_size);
		packets.push(msg);
		i++;
		increment_seq_no(curr_seq_no, seq_field_len);
		// usleep(1000000/pkt_gen_rate);
	}
}

void send_packets(int sockfd, struct sockaddr_in server_addr, std::queue<int *> &buffer_out, int pkt_len, std::queue<int *> &retrans_buf, int window_size) {
	while(true) {
		if(!buffer_out.empty() && retrans_buf.size() < window_size) {
			int *msg = buffer_out.front();
			buffer_out.pop();
			std::cout<<"Sending packet "<<*msg<<std::endl; // remove later
			socklen_t len = sizeof(server_addr);
			sendto(sockfd, (int *) msg, pkt_len*sizeof(int), 0, (const struct sockaddr *) &server_addr, len);
			retrans_buf.push(msg);
		}
	}
}

void recv_packets(int sockfd, std::queue<int *> &retrans_buf, int seq_field_len) {
	int *buffer = new int[seq_field_len];
	int n;
	struct sockaddr_in server_addr;
	socklen_t len = sizeof(server_addr);
	while(true) {
		n = recvfrom(sockfd, (int *) buffer, seq_field_len*sizeof(int), 0, (struct sockaddr *) &server_addr, &len);
		if(!retrans_buf.empty()) {
			if(pkt_in_window(buffer, retrans_buf.front(), retrans_buf.size(), seq_field_len)) {
				while(!retrans_buf.empty()) {
					int *ackd_pkt = retrans_buf.front();
					retrans_buf.pop();
					if(compare_seq_no(buffer, ackd_pkt, seq_field_len))
						break;
				}
			}
		}
		// while(!retrans_buf.empty()) { // handle properly by checking if the ackd packet is in the window
		// 	int *ackd_pkt = retrans_buf.front();
		// 	retrans_buf.pop();
		// 	if(compare_seq_no(buffer, ackd_pkt))
		// 		break;
		// }
		std::cout<<"Received ACK for "<<*buffer<<std::endl; // remove later
	}
}

// Driver code
int main(int argc, char *argv[]) {
	srand(time(0));

	bool debug = false;
	char *recvr_addr;
	int recvr_port, pkt_len, pkt_gen_rate, max_pkts, window_size, max_buf_size;
	int sockfd;
	char buffer[MAXLINE];
	const char *hello = "Hello from client";
	struct sockaddr_in	 servaddr;
	std::queue<int *> packets, retrans_buffer;
	int seq_no_len = 0;
	long long max_window_size = 1;

	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-d")) {
			debug = true;
		}
		else if(!strcmp(argv[i], "-s")) {
			strcpy(recvr_addr, argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-p")) {
			recvr_port = atoi(argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-l")) {
			pkt_len = atoi(argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-r")) {
			pkt_gen_rate = atoi(argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-n")) {
			max_pkts = atoi(argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-w")) {
			window_size = atoi(argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-f")) {
			max_buf_size = atoi(argv[i+1]);
			i++;
		}
	}

	while(max_window_size < window_size) {
		seq_no_len++;
		max_window_size *= 256;
	}
	int *seq_no_gen = new int[seq_no_len];
	for(int i = 0; i < seq_no_len; i++)
		seq_no_gen[i] = 0;

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(recvr_port);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	int *info_pkt = new int[2];
	info_pkt[0] = pkt_len;
	info_pkt[1] = seq_no_len;
	socklen_t len = sizeof(servaddr);
	sendto(sockfd, (int *)info_pkt, 2*sizeof(int), 0, (const sockaddr *) &servaddr, len);

	sleep(1);

	free(info_pkt);

	std::thread thread_gen(gen_packets, std::ref(packets), 9, pkt_gen_rate, pkt_len, seq_no_len, max_buf_size, seq_no_gen);
	std::thread thread_send(send_packets, sockfd, servaddr, std::ref(packets), pkt_len, std::ref(retrans_buffer), window_size);
	std::thread thread_recv(recv_packets, sockfd, std::ref(retrans_buffer), seq_no_len);

	thread_gen.join();
	thread_send.join();
	thread_recv.join();

	close(sockfd);
	return 0;
}
