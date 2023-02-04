/* UDPmathd.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
/* #include <math.h> */

int	passiveUDP(const char *service);
int	errexit(const char *format, ...);
/* double sqrt(double __x); */

int
passivesock(const char *service, const char *transport, int qlen);

/*------------------------------------------------------------------------
 * main - Iterative UDP server for MATH service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	struct sockaddr_in fsin;	/* the from address of a client	*/
	char	*service = "time";	/* service name or port number	*/
	char	buf[32];			/* "input" buffer; any size > 0	*/
	int	sock;			/* server socket		*/
	int	result;			/* current time			*/
	unsigned int	alen;		/* from-address length		*/
	char op[4];
	int op1, op2;
	char temp[32];
	int space1 = -1, space2 = -1;
	int i;
	char unknown_op_error[64];


	switch (argc) {
	case	1:
		break;
	case	2:
		service = argv[1];
		break;
	default:
		errexit("usage: UDPtimed [port]\n");
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

	  for(i = 0; buf[i]; i++)
		if(buf[i] == ' ') {
			if(space1 == -1)
				space1 = i;
			else
				space2 = i;
		}
	
	  printf("Command received: %s\n", buf);
	
	  memset(op, 0, sizeof(op));
	  strncpy(op, buf, space1);
	  printf("Operation: %s\n", op);

	  memset(temp, 0, sizeof(temp));
	  strncpy(temp, buf+space1+1, space2-space1-1);
	  op1 = atoi(temp);
	  printf("Operand 1: %d\n", op1);

	  memset(temp, 0, sizeof(temp));
	  strncpy(temp, buf+space2+1, i-space2-1);
	  op2 = atoi(temp);
	  printf("Operand 2: %d\n", op2);

	  if(!strcmp(op, "add"))
		result = op1 + op2;
	  else if(!strcmp(op, "mul")) 
	  	result = op1 * op2;
	  else if(!strcmp(op, "mod"))
	    result = op1 % op2;
	  else if(!strcmp(op, "hyp"))
/* 		result = (int) sqrt(op1*op1 + op2*op2); */
		result = op1*op1 + op2*op2;
		/* Try default handling */

	  result = htonl((int)result);
		
	  printf("Sending result...\n");
	  (void) sendto(sock, (char*)&result, sizeof(result), 0,
			(struct sockaddr *)&fsin, sizeof(fsin));

	  printf("************** \n");
	}
}
