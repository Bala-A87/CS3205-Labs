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

void recv_packets(int sockfd, std::queue<int *> &buffer, std::queue<struct sockaddr_in> &senders, int pkt_len) {
	int n;
	struct sockaddr_in sender_addr;
	socklen_t len = sizeof(sender_addr);
	while(true) {
		int *packet = new int[pkt_len];
		n = recvfrom(sockfd, (int *)packet, pkt_len*sizeof(int), 0, (struct sockaddr *) &sender_addr, &len);
		std::cout<<"Received packet "<<packet[0]<<std::endl; // remove later
		buffer.push(packet);
		senders.push(sender_addr);
	}
}

void send_packets(int sockfd, std::queue<int *> &buffer, std::queue<struct sockaddr_in> &senders, int seq_field_len) {
	while(true) {
		if(!buffer.empty()) {
			int *recvd_packet = buffer.front();
			struct sockaddr_in sender = senders.front();
			socklen_t len = sizeof(sender);
			buffer.pop();
			senders.pop();
			std::cout<<"Sending ACK message for "<<recvd_packet[0]<<std::endl; // remove later
			int *ack_msg = get_ack_packet(recvd_packet, seq_field_len);
			int n = sendto(sockfd, (const int *) ack_msg, seq_field_len*sizeof(int), 0, (const struct sockaddr *) &sender, len);
			// std::cout<<std::strerror(errno)<<std::endl; ^---------handle
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

	std::thread thread_recv(recv_packets, sockfd, std::ref(recv_buffer), std::ref(senders), pkt_len);
	std::thread thread_send(send_packets, sockfd, std::ref(recv_buffer), std::ref(senders), seq_no_len);

	thread_recv.join();
	thread_send.join();
	
	return 0;
}
