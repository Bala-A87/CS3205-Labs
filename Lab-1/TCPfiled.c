/* TCPfiled.c - main, TCPfiled */

#define	_USE_BSD
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define	QLEN		  32	/* maximum connection queue length	*/
#define	BUFSIZE		4096

void	reaper(int);
int	TCPfiled(int fd);
int	errexit(const char *format, ...);
int	passivesock(const char *service, const char *transport, int qlen);

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for FILE service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*service = "file";	/* service name or port number	*/
	struct	sockaddr_in fsin;	/* the address of a client	*/
	unsigned int	alen;		/* length of client's address	*/
	int	msock;			/* master server socket		*/
	int	ssock;			/* slave server socket		*/

	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		errexit("usage: TCPfiled [port]\n");
	}

	msock = passivesock(service, "tcp", QLEN);

	(void) signal(SIGCHLD, reaper);

	while (1) {
	  alen = sizeof(fsin);
	  ssock = accept(msock, (struct sockaddr *)&fsin, &alen);

	  if (ssock < 0) {
			if (errno == EINTR)
				continue;
			errexit("accept: %s\n", strerror(errno));
		}

	  switch (fork()) {

	  case 0:		/* child */
	    (void) close(msock);
	    exit(TCPfiled(ssock));

	  default:	/* parent */
	    (void) close(ssock);
	    break;

		case -1:
			errexit("fork: %s\n", strerror(errno));
		}
	}
}

/*------------------------------------------------------------------------
 * TCPfiled - print data from a file
 *------------------------------------------------------------------------
 */
int
TCPfiled(int s)
{
	char	buf[BUFSIZ], result[BUFSIZ]; /* Input buffer, output buffer */
	int	cc;
	struct sockaddr_in clientaddr;
	socklen_t len;
	char filename[32]; /* File name received */
	int bytestoread, spaceloc, i; /* Number of bytes to read (input), location of space, iterator variable */
	int filedesc; /* File descriptor of required file */

	len = sizeof(clientaddr);
	getpeername(s, (struct sockaddr*)&clientaddr, &len);

	printf("************** \n");
	printf("Client IP address: %s\n", inet_ntoa(clientaddr.sin_addr));
	printf("Client port      : %d\n", ntohs(clientaddr.sin_port));
	printf("************** \n");
	
	memset(buf, 0, sizeof(buf));

	while(cc = read(s, buf, sizeof(buf))) {
	  if (cc < 0)
	    errexit("read from client: %s\n", strerror(errno));
	  if (!strcmp(buf, "\0")) /* Sometimes, phantom (null character) reads occur, which is handled here by ignoring them */
	  	continue;
	
	  printf("\n");

	  for(i = 0; buf[i]; i++) { /* Find location of space to extract file name and number of bytes */
		if(buf[i] == ' ') {
			spaceloc = i;
			break;
		}
	  }
	  memset(filename, 0, sizeof(filename));
	  strncpy(filename, buf, spaceloc);  /* String before space = file name */
	  bytestoread = atoi(buf+spaceloc+1); /* String after space (converted to int) = bytes to read */

	  memset(result, 0, sizeof(result));
	  filedesc = open(filename, O_RDONLY);
	  if(filedesc < 0) { /* File not found */
		strcpy(result, "SORRY!"); /* Special result string */
		printf("File %s not found\n", filename);
	  }
	  else {
	  	printf("Reading %d bytes from the end of file %s\n", bytestoread, filename);
		/* Position file pointer to required position from end of file, then read */
		cc = pread(filedesc, result, bytestoread, lseek(filedesc, -bytestoread, SEEK_END)); 
		if (cc < 0)
	    	errexit("read from file: %s\n", strerror(errno));
		close(filedesc);
	  }

	  memset(buf, 0, sizeof(buf));
	  if(write(s, result, bytestoread) < 0)
	 	errexit("write to client: %s\n", strerror(errno));
	  printf("Read: %s\n", result);
	}
	return 0;
}

/*------------------------------------------------------------------------
 * reaper - clean up zombie children
 *------------------------------------------------------------------------
 */
void
reaper(int sig)
{
	int	status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		/* empty */;
}
