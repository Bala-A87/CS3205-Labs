/* TCPfile.c - main, TCPfile */

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

void	TCPfile(const char *host, const char *service);
int	errexit(const char *format, ...);

int	connectsock(const char *host, const char *service,
		const char *transport);

#define	LINELEN		8192
#define FNFSTRING "SORRY!"

/*------------------------------------------------------------------------
 * main - TCP client for FILE service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*service = "file";	/* default service name		*/

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
		fprintf(stderr, "usage: TCPfile [host [port]]\n");
		exit(1);
	}

	TCPfile(host, service);

	exit(0);
}

/*------------------------------------------------------------------------
 * TCPfile - send input to FILE service on specified host and print reply
 *------------------------------------------------------------------------
 */
void 
TCPfile(const char *host, const char *service)
{
	char	buf[LINELEN+1];		/* buffer for one line of text	*/
	int	s, n;			/* socket descriptor, read count*/
	int	inchars, i;	/* characters sent and received	*/
	struct sockaddr_in serveraddr;
	socklen_t len;
	int bytestoread, space, new_fd;
	size_t bytestocompare; /* number of bytes to compare if file not found */
	/* Received number of bytes, location of space, new file descriptor */
	char filename[32]; /* Received file name */

	s = 	connectsock(host, service, "tcp");
	if (s < 0)
	  {
		fprintf(stderr, "usage: TCPfile connectsock failed. \n");
		exit(1);
	  }
	
	len = sizeof(serveraddr);
	getpeername(s, (struct sockaddr*)&serveraddr, &len);

	printf("************** \n");
	printf("Server IP address: %s\n", inet_ntoa(serveraddr.sin_addr));
	printf("Server port      : %d\n", ntohs(serveraddr.sin_port));
	printf("************** \n");

	memset(buf, 0, sizeof(buf));

	while (fgets(buf, sizeof(buf), stdin)) {
		buf[LINELEN] = '\0';	/* insure line null-terminated	*/
		(void) write(s, buf, sizeof(buf));

		for(i = 0; buf[i]; i++) /* Find location of space to extract file name and number of bytes */
			if(buf[i] == ' ') {
				space = i;
				break;
			}
		bytestoread = atoi(buf+space+1); /* String (converted to integer) after space = bytes to read */
		memset(filename, 0, sizeof(filename));
		strncpy(filename, buf, space); /* String until before space = file name */

		memset(buf, 0, sizeof(buf));

		/* read it back */

		for (inchars = 0; inchars < bytestoread; inchars+=n ) { /* Read bytestoread number of bytes */
			n = read(s, &buf[inchars], bytestoread - inchars);
			if (n < 0)
				errexit("socket read failed: %s\n",
					strerror(errno));
		}
		bytestocompare = strlen(FNFSTRING);
		if(strlen(buf) < bytestocompare)
			bytestocompare = strlen(buf);
		/* If file doesn't exist, result may contain a substring of FNFSTRING, so determine how many characters to compare */
		if(!strncmp(FNFSTRING, buf, bytestocompare)) 
			printf("Server says that the file does not exist.\n"); /* File not found, result is substring of FNFSTRING */
		else
			printf("Content read: %s\n", buf);
		new_fd = creat(strcat(filename, "1"), 0666); /* Create filename1 with RW permissions */
		write(new_fd, buf, strlen(buf));
		close(new_fd);
		memset(buf, 0, sizeof(buf));
		printf("\n");
	}
}
