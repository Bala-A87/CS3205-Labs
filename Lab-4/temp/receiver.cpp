// Server side implementation of UDP client-server model
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

int *get_ack_packet(int *data_pkt, int seq_field_len) {
	int *ack_pkt = new int[seq_field_len];
	for(int j = 0; j < seq_field_len; j++)
		ack_pkt[j] = data_pkt[j];
	return ack_pkt;
}

bool equal_seq_no(int *packet, int *seq_no, int seq_no_len) {
	for(int i = 0; i < seq_no_len; i++)
		if(packet[i] != seq_no[i])
			return false;
	return true;
}

void copy_seq_no(int *seq_no_field, int *pkt, int seq_no_len) {
	for(int i = 0; i < seq_no_len; i++)
		seq_no_field[i] = pkt[i];
}

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

void recv_packets(int sockfd, std::queue<int *> &buffer, std::queue<struct sockaddr_in> &senders, int pkt_len, int seq_field_len, int *next_pkt_exp, int *last_pkt_ackd, float pkt_drop_proba) {
	int n;
	struct sockaddr_in sender_addr;
	socklen_t len = sizeof(sender_addr);
	while(true) {
		int *packet = new int[pkt_len];
		n = recvfrom(sockfd, (int *)packet, pkt_len*sizeof(int), 0, (struct sockaddr *) &sender_addr, &len);
		std::cout<<"Received packet "<<packet[0]<<std::endl; // remove later
		bool accept = equal_seq_no(packet, next_pkt_exp, seq_field_len);
		// buffer.push(packet);
		bool drop_err = (rand() % ((int) (1 / pkt_drop_proba))) == 0;
		if(!drop_err) {
			senders.push(sender_addr); // add drop proba
			if(accept) {
				increment_seq_no(next_pkt_exp, seq_field_len);
				buffer.push(get_ack_packet(packet, seq_field_len));
				copy_seq_no(last_pkt_ackd, packet, seq_field_len);
			}
			else {
				buffer.push(get_ack_packet(last_pkt_ackd, seq_field_len));
			}
		}
		else 
			std::cout<<"Oops! Packet "<<packet[0]<<" dropped!"<<std::endl; //remove later
		free(packet);
	}
}

void send_packets(int sockfd, std::queue<int *> &buffer, std::queue<struct sockaddr_in> &senders, int seq_field_len) {
	while(true) {
		if(!buffer.empty()) {
			int *ack_msg = buffer.front();
			struct sockaddr_in sender = senders.front();
			socklen_t len = sizeof(sender);
			buffer.pop();
			senders.pop();
			std::cout<<"Sending ACK message for "<<ack_msg[0]<<std::endl; // remove later
			// int *ack_msg = get_ack_packet(recvd_packet, seq_field_len);
			int n = sendto(sockfd, (const int *) ack_msg, seq_field_len*sizeof(int), 0, (const struct sockaddr *) &sender, len);
			// std::cout<<std::strerror(errno)<<std::endl; 
			free(ack_msg);
		}
	}
}

// Driver code
int main(int argc, char *argv[]) {
	srand(time(0));

	bool debug = false;
	int recvr_port, max_pkts;
	float pkt_error_rate;
	int sockfd;
	char buffer[MAXLINE];
	const char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
	std::queue<int *> recv_buffer;
	std::queue<struct sockaddr_in> senders;

	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-d")) {
			debug = true;
		}
		else if(!strcmp(argv[i], "-p")) {
			recvr_port = atoi(argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-n")) {
			max_pkts = atoi(argv[i+1]);
			i++;
		}
		else if(!strcmp(argv[i], "-e")) {
			pkt_error_rate = atof(argv[i+1]);
			i++;
		}
	}

	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(recvr_port);
	
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	int *info_buf = new int[2];
	socklen_t len = sizeof(servaddr);
	int n = recvfrom(sockfd, (int *) info_buf, 2*sizeof(int), 0, (sockaddr *) &servaddr, &len);
	int pkt_len = info_buf[0];
	int seq_no_len = info_buf[1];
	free(info_buf);

	int *next_pkt_exp = new int[seq_no_len];
	for(int i = 0; i < seq_no_len; i++)
		next_pkt_exp[i] = 0;

	int *last_pkt_ackd = new int[seq_no_len];
	for(int i = 0; i < seq_no_len; i++)
		last_pkt_ackd[i] = 256;

	std::thread thread_recv(recv_packets, sockfd, std::ref(recv_buffer), std::ref(senders), pkt_len, seq_no_len, next_pkt_exp, last_pkt_ackd, pkt_error_rate);
	std::thread thread_send(send_packets, sockfd, std::ref(recv_buffer), std::ref(senders), seq_no_len);

	thread_recv.join();
	thread_send.join();
	
	return 0;
}
