/* UDPmath.c - main */

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

#define	UNIXEPOCH	2208988800UL	/* UNIX epoch, in UCT secs	*/

int	errexit(const char *format, ...);

int
connectsock(const char *host, const char *service, const char *transport );



/*--------------------------------------------------------------------------------------
 * main - UDP client for MATH service that prints the results of particular operations
 *--------------------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*service = "math";	/* default service name		*/
	int	s, n;			/* socket descriptor, read count*/
	char buf[32]; /* input buffer */
	int result; /* result of operation */
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
		fprintf(stderr, "usage: UDPtime [host [port]]\n");
		exit(1);
	}

	s = connectsock(host, service, "udp");

	len = sizeof(serveraddr);
	getpeername(s, (struct sockaddr*)&serveraddr, &len);

	printf("************** \n");
	printf("Server IP address: %s\n", inet_ntoa(serveraddr.sin_addr));
	printf("Server port      : %d\n", ntohs(serveraddr.sin_port));
	printf("************** \n");
	
	printf("Enter command: ");
	scanf("%[^\n]s", buf);

	(void) write(s, buf, sizeof(buf));

	/* Read the result */

	n = read(s, (char *)&result, sizeof(result));
	if (n < 0)
		errexit("read failed: %s\n", strerror(errno));
	result = ntohl((int)result);	
	printf("%d\n", result);

	exit(0);
}
