/* UDPecho.c - main */

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#define	BUFSIZE 64

int	errexit(const char *format, ...);

int
connectsock(const char *host, const char *service, const char *transport );



/*----------------------------------------------------------------------------------------------
 * main - UDP client for ECHO service that receives message sent to server back and prints it
 *----------------------------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*service = "echo";	/* default service name		*/
	char buf[1024]; /* buffer to hold the message sent and received */
	int	s, n;			/* socket descriptor, read count*/
	struct sockaddr_in serveraddr;
	socklen_t len;
 
	switch (argc) {
	case 1:
		host = "localhost";
		break;
	case 3:
		service = argv[2];
		/* FALL THROUGH */
	case 2:
		host = argv[1];
		break;
	default:
		fprintf(stderr, "usage: UDPecho [host [port]]\n");
		exit(1);
	}

	s = connectsock(host, service, "udp");

	len = sizeof(serveraddr);
	getpeername(s, (struct sockaddr*)&serveraddr, &len);

	printf("************** \n");
	printf("Server IP address: %s\n", inet_ntoa(serveraddr.sin_addr));
	printf("Server port      : %d\n", ntohs(serveraddr.sin_port));
	printf("************** \n");

	printf("Enter the message: ");
	scanf("%[^\n]s", buf); /* Allowing for spaces also to be read */
	
	(void) write(s, buf, sizeof(buf));

	/* Read the echoed message */

	n = read(s, (char *)&buf, sizeof(buf));
	if (n < 0)
		errexit("read failed: %s\n", strerror(errno));

	printf("Message echoed: %s\n", buf);
	exit(0);
}
