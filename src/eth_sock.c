/*
Here we do all the sockethandling like creating a socket listener,
a socket fd for connecting to the other side etc...
*/

/**
 * TODO: Every fdo should have its own buffer to read/send data
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <resolv.h>

#include <termios.h>
#include <sys/stat.h>

#include <servercore/log_writer.h>


/*Create a socket fd to connect to another socket*/
int32_t connect_socket(unsigned char * ipaddr, int32_t portnr, int32_t protocol)
{
	/*create a socket fd*/
	int32_t clientfd;
	struct sockaddr_in clientaddr;

	if((*ipaddr == '\0') || portnr < 1024)
		return -1;	//error

	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Client socket");
		return -1;
	}

	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(portnr);
	if(inet_aton((const char*)ipaddr, (struct in_addr*)&clientaddr.sin_addr.s_addr) == 0)
	{
		perror("inet_aton");
		return -1;
	}

	if(connect(clientfd, (struct sockaddr*) &clientaddr, sizeof(clientaddr)) != 0)
	{
		perror("Client connect");
		return -1;
	}

	return clientfd;
}

int32_t send_to_client(int fd, char* data, int32_t len)
{
	int32_t sentbytes = 0;

	do {
		sentbytes = send(fd, data, len, 0);
		if(sentbytes == -1)
			return -1;
		len -= sentbytes;
	} while(len);

	return 0;
}


int32_t create_listener(char* ip, int port, int type, int protocol, int backlog)
{
    int fd;
    int on = 1;
    int32_t res = 0;

	struct sockaddr_in serveraddr_tcp = {0};

	serveraddr_tcp.sin_family = AF_INET;
	serveraddr_tcp.sin_port = htons(port);
	if(ip) {
        /* convert ip to long */
        serveraddr_tcp.sin_addr.s_addr = inet_addr(ip);
	} else serveraddr_tcp.sin_addr.s_addr = INADDR_ANY; //ip


	fd = socket(AF_INET, type, protocol);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	res = bind(fd, (struct sockaddr*) &serveraddr_tcp, sizeof(serveraddr_tcp));
	if(res < 0) {
		DEBUG_MSG("could not bind serversocket to local addres: %s", strerror(errno));
		return -1;
	}

	res = listen(fd, backlog);
	if(res < 0) {
		DEBUG_MSG("failed to listen for incoming requests: %s", strerror(errno));
		return -1;
	}

	return fd;
}
