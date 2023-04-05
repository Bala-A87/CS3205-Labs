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

bool seq_no_geq(int seq1, int seq2) {
	if(seq1 == 256 || seq2 == 256)
		return false;
	else 
		return seq1 >= seq2;
}

std::pair<int, int> get_rtt_m_u(long long rtt) {
	return std::pair<int, int>({rtt/1000, rtt%1000});
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

int get_timeout_time(long long rtt_sum, long long rtt_count) {
	if(rtt_count < 10)
		return 100000;
	return 2000 * ((rtt_sum / rtt_count)/1000 + 1);
}

// Timeout handler
void wait_for_timeout(long long wait_time_us, bool &timeout, std::thread **wait_threads, int seq_no, bool *complete) {
	std::this_thread::sleep_for(std::chrono::microseconds(wait_time_us));
	if(std::this_thread::get_id() == wait_threads[seq_no]->get_id() && !complete[seq_no]) {
		timeout = true;
	}
}

void gen_packets(std::queue<int *> &packets, int num, int pkt_gen_rate, int pkt_len, int max_buffer_size, int &curr_seq_no, bool &stop, long long *gen_time, std::chrono::_V2::system_clock::time_point start_time) {
	int i = 0;
	while(!stop) {
		int *msg = new int[pkt_len];
		msg[0] = curr_seq_no;
		for(int j = 1; j < pkt_len; j++)
			msg[j] = rand()%256;
		while(packets.size() >= max_buffer_size);
		gen_time[curr_seq_no] = (long long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
		packets.push(msg);
		i++;
		curr_seq_no++;
		curr_seq_no %= 256;
		std::this_thread::sleep_for(std::chrono::microseconds(1000000/pkt_gen_rate));
	}
}

void send_packets(int sockfd, struct sockaddr_in server_addr, std::queue<int *> &buffer_out, int pkt_len, std::queue<int *> &retrans_buf, int window_size, std::chrono::_V2::system_clock::time_point *start_time, bool &timeout, std::thread **wait_threads, bool *complete, long long &rtt_sum, long long &rtt_count, bool &stop, int *retrans_attempts, int &total_trans) {
	while(!stop) {
		if(timeout) {
			// Retrans packets
			for(int i = 0; i < retrans_buf.size(); i++) {
				int *msg = retrans_buf.front();
				retrans_buf.pop();
				if(retrans_attempts[*msg] >= 5) {
					std::cout<<"Packet "<<*msg<<" has been retransmitted 5 times, ending connection"<<"\n";
					stop = true;
					break;
				}
				socklen_t len = sizeof(server_addr);
				sendto(sockfd, (int *) msg, pkt_len*sizeof(int), 0, (const struct sockaddr *) &server_addr, len);
				start_time[*msg] = std::chrono::high_resolution_clock::now();
				wait_threads[*msg] = new std::thread(wait_for_timeout, get_timeout_time(rtt_sum, rtt_count), std::ref(timeout), wait_threads, *msg, complete);
				total_trans++;
				retrans_attempts[*msg]++;
				retrans_buf.push(msg);
			}
			timeout = false;
		}
		if(stop)
			break;
		if(!buffer_out.empty() && retrans_buf.size() < window_size) {
			// Send new packets
			int *msg = buffer_out.front();
			buffer_out.pop();
			socklen_t len = sizeof(server_addr);
			complete[*msg] = false;
			retrans_attempts[*msg] = 0;
			sendto(sockfd, (int *) msg, pkt_len*sizeof(int), 0, (const struct sockaddr *) &server_addr, len);
			start_time[*msg] = std::chrono::high_resolution_clock::now();
			wait_threads[*msg] = new std::thread(wait_for_timeout, get_timeout_time(rtt_sum, rtt_count), std::ref(timeout), wait_threads, *msg, complete);
			total_trans++;
			retrans_buf.push(msg);
		}
	}
	// Cleanup, let receiver stop threads and exit
	int *stop_msg = new int[pkt_len];
	stop_msg[0] = 256;
	socklen_t len = sizeof(server_addr);
	sendto(sockfd, (int *) stop_msg, pkt_len*sizeof(int), 0, (const struct sockaddr *) &server_addr, len);
	sleep(1);
	free(stop_msg);
	while(!buffer_out.empty()) {
		free(buffer_out.front());
		buffer_out.pop();
	}
}

void recv_packets(int sockfd, std::queue<int *> &retrans_buf, std::chrono::_V2::system_clock::time_point *start_time, long long &rtt_sum, long long &rtt_count, bool *complete, bool &stop, bool debug, long long *gen_time, int *retrans_attempts, int max_pkts) {
	int *buffer = new int;
	int n;
	struct sockaddr_in server_addr;
	socklen_t len = sizeof(server_addr);
	while(!stop) {
		n = recvfrom(sockfd, (int *) buffer, sizeof(int), 0, (struct sockaddr *) &server_addr, &len);
		auto end_time = std::chrono::high_resolution_clock::now();
		if(*buffer == 256) {
			stop = true;
			std::cout<<"Connection terminated by receiver"<<"\n";
			break;
		}
		if(!retrans_buf.empty()) {
			if(pkt_in_window(*buffer, retrans_buf.front()[0], retrans_buf.size())) {
				// Remove from retrans buffer
				if(*retrans_buf.front() <= *buffer)
					for(int i = *retrans_buf.front(); i <= *buffer; i++)
						complete[i] = true;
				else {
					for(int i = *retrans_buf.front(); i < 256; i++)
						complete[i] = true;
					for(int i = 0; i <= *buffer; i++)
						complete[i] = true;
				}
				while(!retrans_buf.empty()) {
					int *ackd_pkt = retrans_buf.front();
					long long rtt = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time[*ackd_pkt]).count();
					std::pair<int, int> gen_time_mu = get_rtt_m_u(gen_time[*ackd_pkt]);
					std::pair<int, int> rtt_mu = get_rtt_m_u(rtt);
					if(debug)
						std::cout<<"Seq #:"<<*ackd_pkt<<"\t Time Generated: "<<gen_time_mu.first<<":"<<gen_time_mu.second<<" RTT: "<<rtt_mu.first<<":"<<rtt_mu.second<<"\t Number of Attempts: "<<retrans_attempts[*ackd_pkt]+1<<"\n";
					rtt_sum += rtt;
					rtt_count++;
					retrans_buf.pop();
					if(rtt_count >= max_pkts) {
						stop = true;
						std::cout<<max_pkts<<" packets acknowledged"<<"\n";
						break;
					}
					if(ackd_pkt[0] == buffer[0]) {
						free(ackd_pkt);
						break;
					}
					free(ackd_pkt);
				}
			}
		}
	}
	free(buffer);
}

// Driver code
int main(int argc, char *argv[]) {
	srand(time(0));

	bool debug = false;
	char recvr_addr[64];
	strcpy(recvr_addr, "127.0.0.1");
	int recvr_port = 9999, pkt_len = 256, pkt_gen_rate = 100, max_pkts = 500, window_size = 64, max_buf_size = 200;
	int sockfd;
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
	servaddr.sin_addr.s_addr = inet_addr(recvr_addr);

	int *info_pkt = new int;
	*info_pkt = pkt_len;
	socklen_t len = sizeof(servaddr);
	sendto(sockfd, (int *)info_pkt, sizeof(int), 0, (const sockaddr *) &servaddr, len);

	sleep(1);

	std::vector<std::thread *> dummy;
	free(info_pkt);
	std::chrono::_V2::system_clock::time_point start_time[256], end_time[256];
	std::thread **timeout_threads = new std::thread*[256];
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
	int total_trans = 0;
	long long gen_times[256];

	std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::thread thread_gen(gen_packets, std::ref(packets), max_pkts, pkt_gen_rate, pkt_len, max_buf_size, std::ref(seq_no), std::ref(stop_comm), gen_times, start);
	std::thread thread_send(send_packets, sockfd, servaddr, std::ref(packets), pkt_len, std::ref(retrans_buffer), window_size, start_time, std::ref(timeout), timeout_threads, complete, std::ref(rtt_sum), std::ref(rtt_count), std::ref(stop_comm), retrans_attempts, std::ref(total_trans));
	std::thread thread_recv(recv_packets, sockfd, std::ref(retrans_buffer), start_time, std::ref(rtt_sum), std::ref(rtt_count), complete, std::ref(stop_comm), debug, gen_times, retrans_attempts, max_pkts);

	thread_gen.join();
	thread_send.join();
	thread_recv.join();

	close(sockfd);

	std::pair<int, int> final_avg_rtt = get_rtt_m_u(rtt_sum/rtt_count);
	std::cout<<"Pkt gen rate: "<<pkt_gen_rate<<", pkt len: "<<pkt_len<<", retrans ratio: "<<((double) total_trans)/rtt_count<<", avg RTT value: "<<final_avg_rtt.first<<":"<<final_avg_rtt.second<<"\n";
	free(timeout_threads);

	return 0;
}
