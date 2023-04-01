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

bool seq_no_geq(int seq1, int seq2) {
	if(seq1 == 256 || seq2 == 256)
		return false;
	else 
		return seq1 >= seq2;
}

bool pkt_in_window(int ack_seq_no, int window_start, int window_size) {
	int window_end = window_start + window_size;
	int carry = 0;
	if(window_end >= 256) {
		carry =  window_end / 256;
		window_end %= 256;
	}
	bool ans = false;
	if(seq_no_geq(ack_seq_no, window_start) && seq_no_geq(window_end, ack_seq_no))
		ans = true;
	else if(seq_no_geq(ack_seq_no, window_start) && (!seq_no_geq(window_end, ack_seq_no) && carry > 0))
		ans = true;
	else if((!seq_no_geq(ack_seq_no, window_start) && carry > 0) && seq_no_geq(window_end, ack_seq_no))
		ans = true;
	return ans;
}

int get_timeout_time(int pkts_sent, long long rtt_sum, long long rtt_count) {
	if(pkts_sent < 10)
		return 100000;
	return 2 * (rtt_sum / rtt_count);
}

void wait_for_timeout(long long wait_time_us, bool &timeout, std::thread **wait_threads, int seq_no, bool *complete) {
	usleep(wait_time_us);
	if(std::this_thread::get_id() == wait_threads[seq_no]->get_id() && !complete[seq_no])
		timeout = true;
}

void gen_packets(std::queue<int *> &packets, int num, int pkt_gen_rate, int pkt_len, int max_buffer_size, int &curr_seq_no, bool &stop) {
	int i = 0;
	while(!stop) {
		if(i >= num) {
			stop = true;
			break;
		}
		int *msg = new int[pkt_len];
		msg[0] = curr_seq_no;
		for(int j = 1; j < pkt_len; j++)
			msg[j] = rand()%256;
		while(packets.size() >= max_buffer_size);
		packets.push(msg);
		i++;
		curr_seq_no++;
		usleep(1000000/pkt_gen_rate);
	}

}

void send_packets(int sockfd, struct sockaddr_in server_addr, std::queue<int *> &buffer_out, int pkt_len, std::queue<int *> &retrans_buf, int window_size, std::chrono::_V2::system_clock::time_point *start_time, bool &timeout, std::thread **wait_threads, bool *complete, int &pkts_sent, long long &rtt_sum, long long &rtt_count, bool &stop, int *retrans_attempts) {
	while(!stop) {
		if(timeout) {
			for(int i = 0; i < retrans_buf.size(); i++) {
				int *msg = retrans_buf.front();
				retrans_buf.pop();
				if(retrans_attempts[*msg] > 5) {
					std::cout<<"Packet "<<*msg<<" has been retransmitted 5 times, ending connection"<<std::endl;
					stop = true;
					break;
				}
				std::cout<<"Retransmitting packet "<<*msg<<std::endl; // remove later
				socklen_t len = sizeof(server_addr);
				complete[*msg] = false;
				sendto(sockfd, (int *) msg, pkt_len*sizeof(int), 0, (const struct sockaddr *) &server_addr, len);
				start_time[*msg] = std::chrono::high_resolution_clock::now();
				wait_threads[*msg] = new std::thread(wait_for_timeout, get_timeout_time(pkts_sent, rtt_sum, rtt_count), std::ref(timeout), wait_threads, *msg, complete);
				pkts_sent++;
				retrans_attempts[*msg]++;
				retrans_buf.push(msg);
			}
			timeout = false;
		}
		if(stop)
			break;
		if(!buffer_out.empty() && retrans_buf.size() < window_size) {
			int *msg = buffer_out.front();
			buffer_out.pop();
			std::cout<<"Sending packet "<<*msg<<std::endl; // remove later
			socklen_t len = sizeof(server_addr);
			complete[*msg] = false;
			retrans_attempts[*msg] = 0;
			sendto(sockfd, (int *) msg, pkt_len*sizeof(int), 0, (const struct sockaddr *) &server_addr, len);
			start_time[*msg] = std::chrono::high_resolution_clock::now();
			wait_threads[*msg] = new std::thread(wait_for_timeout, get_timeout_time(pkts_sent, rtt_sum, rtt_count), std::ref(timeout), wait_threads, *msg, complete);
			pkts_sent++;
			retrans_buf.push(msg);
		}
	}
	int *stop_msg = new int[pkt_len];
	stop_msg[0] = 256;
	socklen_t len = sizeof(server_addr);
	sendto(sockfd, (int *) stop_msg, pkt_len*sizeof(int), 0, (const struct sockaddr *) &server_addr, len);
	sleep(1);
	free(stop_msg);
}

void recv_packets(int sockfd, std::queue<int *> &retrans_buf, std::chrono::_V2::system_clock::time_point *end_time, std::chrono::_V2::system_clock::time_point *start_time, long long &rtt_sum, long long &rtt_count, bool *complete, bool &stop) {
	int *buffer = new int;
	int n;
	struct sockaddr_in server_addr;
	socklen_t len = sizeof(server_addr);
	while(!stop) {
		n = recvfrom(sockfd, (int *) buffer, sizeof(int), 0, (struct sockaddr *) &server_addr, &len);
		if(*buffer == 256) {
			stop = true;
			break;
		}
		if(!retrans_buf.empty()) {
			if(pkt_in_window(*buffer, retrans_buf.front()[0], retrans_buf.size())) {
				while(!retrans_buf.empty()) {
					int *ackd_pkt = retrans_buf.front();
					end_time[*ackd_pkt] = std::chrono::high_resolution_clock::now();
					complete[*ackd_pkt] = true;
					std::cout<<"Time for ACK "<<*ackd_pkt<<": "<<(long long)std::chrono::duration_cast<std::chrono::microseconds>(end_time[*ackd_pkt] - start_time[*ackd_pkt]).count()<<std::endl;
					rtt_sum += (long long)std::chrono::duration_cast<std::chrono::microseconds>(end_time[*ackd_pkt] - start_time[*ackd_pkt]).count();
					rtt_count++;
					retrans_buf.pop();
					if(ackd_pkt[0] == buffer[0])
						break;
				}
			}
		}
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
	bool stop_comm = false;

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

	int seq_no = 0;

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

	int *info_pkt = new int;
	*info_pkt = pkt_len;
	socklen_t len = sizeof(servaddr);
	sendto(sockfd, (int *)info_pkt, sizeof(int), 0, (const sockaddr *) &servaddr, len);

	sleep(1);

	free(info_pkt);
	std::chrono::_V2::system_clock::time_point start_time[256], end_time[256];
	std::thread *timeout_threads[256];
	long long rtt_sum = 0;
	long long rtt_count = 0;
	bool timeout = false;
	bool complete[256];
	int retrans_attempts[256];
	for(int i = 0; i < 256; i++) {
		complete[i] = 256;
		retrans_attempts[i] = 0;
	}
	int pkts_sent = 0;

	std::thread thread_gen(gen_packets, std::ref(packets), max_pkts, pkt_gen_rate, pkt_len, max_buf_size, std::ref(seq_no), std::ref(stop_comm));
	std::thread thread_send(send_packets, sockfd, servaddr, std::ref(packets), pkt_len, std::ref(retrans_buffer), window_size, start_time, std::ref(timeout), timeout_threads, complete, std::ref(pkts_sent), std::ref(rtt_sum), std::ref(rtt_count), std::ref(stop_comm), retrans_attempts);
	std::thread thread_recv(recv_packets, sockfd, std::ref(retrans_buffer), end_time, start_time, std::ref(rtt_sum), std::ref(rtt_count), complete, std::ref(stop_comm));

	thread_gen.join();
	thread_send.join();
	thread_recv.join();

	close(sockfd);
	return 0;
}
