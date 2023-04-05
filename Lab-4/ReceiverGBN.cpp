// NAME: Balakrishnan A
// Roll Number: CS20B012
// Course: CS3205 Jan. 2023 semester
// Lab number: 4
// Date of submission: Apr 5, 2023
// I confirm that the source file is entirely written by me without
// resorting to any dishonest means.
// Website(s) that I used for basic socket programming code are:
// URL(s): https://www.geeksforgeeks.org/udp-server-client-implementation-c/

#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void recv_packets(int sockfd, std::queue<int *> &buffer, std::queue<struct sockaddr_in> &senders, int pkt_len, int &next_pkt_exp, int &last_pkt_ackd, float pkt_drop_proba, bool &stop, std::chrono::_V2::system_clock::time_point start_time, bool debug) {
	int n;
	struct sockaddr_in sender_addr;
	socklen_t len = sizeof(sender_addr);
	while(!stop) {
		int *packet = new int[pkt_len];
		n = recvfrom(sockfd, (int *)packet, pkt_len*sizeof(int), 0, (struct sockaddr *) &sender_addr, &len);
		if(*packet == 256) { // kill signal from sender, illegal seq no
			stop = true;
			break;
		}
		long long accept_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
		int accept_ms = accept_time / 1000;
		int accept_us = accept_time % 1000;
		bool accept = (packet[0] == next_pkt_exp); // ack if matches nfe
		bool drop_err = (rand() / (double)RAND_MAX) <= pkt_drop_proba;
		if(!drop_err) {
			senders.push(sender_addr);
			if(accept) {
				if (debug)
					std::cout<<"Seq #: "<<*packet<<" Time Received: "<<accept_ms<<":"<<accept_us<<" Packet dropped: false"<<"\n";
				next_pkt_exp++;
				next_pkt_exp %= 256;
				int *ack_pkt = new int;
				*ack_pkt = packet[0];
				buffer.push(ack_pkt);
				last_pkt_ackd = packet[0];
			}
		}
		else if(accept && debug)
			std::cout<<"Seq #: "<<*packet<<" Time Received: "<<accept_ms<<":"<<accept_us<<" Packet dropped: true"<<"\n";
		free(packet);
	}
}

void send_packets(int sockfd, std::queue<int *> &buffer, std::queue<struct sockaddr_in> &senders, bool &stop, int &recvd_pkts, int max_pkts) {
	struct sockaddr_in sender;
	while(!stop) { // check condn for max packets ackd & stop accordingly
		if(!buffer.empty()) {
			int *ack_msg = buffer.front();
			sender = senders.front();
			socklen_t len = sizeof(sender);
			buffer.pop();
			senders.pop();
			int n = sendto(sockfd, (const int *) ack_msg, sizeof(int), 0, (const struct sockaddr *) &sender, len);
			free(ack_msg);
			recvd_pkts++;
			if(recvd_pkts >= max_pkts) {
				stop = true;
				break;
			}
		}
	}
	// kill signal for sender to break out of recvfrom wait
	int *stop_msg = new int;
	*stop_msg = 256;
	socklen_t len = sizeof(sender);
	int n = sendto(sockfd, (const int *) stop_msg, sizeof(int), 0, (const struct sockaddr *) &sender, len);
	sleep(1);
	free(stop_msg);
}

// Driver code
int main(int argc, char *argv[]) {
	srand(time(0));

	bool debug = false;
	int recvr_port = 9999, max_pkts = 500;
	float pkt_error_rate = 1e-4;
	int sockfd;
	struct sockaddr_in servaddr;
	std::queue<int *> recv_buffer;
	std::queue<struct sockaddr_in> senders;
	bool stop_comm = false;

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

	int *info_buf = new int;
	socklen_t len = sizeof(servaddr);
	int n = recvfrom(sockfd, (int *) info_buf, sizeof(int), 0, (sockaddr *) &servaddr, &len);
	int pkt_len = *info_buf;
	free(info_buf);

	int next_exp_pkt = 0;
	int last_pkt_ackd = 256;
	int recvd_pkts = 0;
	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

	std::thread thread_recv(recv_packets, sockfd, std::ref(recv_buffer), std::ref(senders), pkt_len, std::ref(next_exp_pkt), std::ref(last_pkt_ackd), pkt_error_rate, std::ref(stop_comm), start, debug);
	std::thread thread_send(send_packets, sockfd, std::ref(recv_buffer), std::ref(senders), std::ref(stop_comm), std::ref(recvd_pkts), max_pkts);

	thread_recv.join();
	thread_send.join();
	
	return 0;
}
