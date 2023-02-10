/* UDPechod.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

int	passiveUDP(const char *service);
int	errexit(const char *format, ...);

int
passivesock(const char *service, const char *transport, int qlen);

/*------------------------------------------------------------------------
 * main - Iterative UDP server for ECHO service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	struct sockaddr_in fsin;	/* the from address of a client	*/
	char	*service = "echo";	/* service name or port number	*/
	char	buf[1024];			/* "input" buffer; any size > 0	*/
	int	sock;			/* server socket		*/
	unsigned int	alen;		/* from-address length		*/

	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		errexit("usage: UDPechod [port]\n");
	}

	sock = passivesock(service, "udp", 0);
	/* Last parameter is Queue length and not applicable for UDP sockets*/

	while (1) {
	  int count;
	  alen = sizeof(fsin);


	  count = recvfrom(sock, buf, sizeof(buf), 0,
			   (struct sockaddr *)&fsin, &alen);

	  printf("************** \n");
	  printf("Client IP address: %s\n", inet_ntoa(fsin.sin_addr));
	  printf("Client port      : %d\n", ntohs(fsin.sin_port));
	  
	  if (count < 0)
	    errexit("recvfrom: %s\n", strerror(errno));

	  printf("Message received from client: %s\n", buf);

	  printf("Sending message back to client...\n");
	  (void) sendto(sock, buf, sizeof(buf), 0,
		 (struct sockaddr *)&fsin, sizeof(fsin));

	  printf("************** \n");
	}
}
