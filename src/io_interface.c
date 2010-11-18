/******************************************************************************
 *   El Andaluz
 *
 * Hier doen we ook de afhandeling of een bericht compleet is of niet!
 * De check is gebaseerd op de eerste 3 bytes (genaamd messageheader), nl:
 * begin message marker 		= 0x00		1 byte
 * length of messageheader	=	0xYYZZ	2 bytes (een short variabele)
 * We checken of een bericht compleet verstuurd is en zo niet, dan bewaren in
 * een "emmer". Is bericht wel compleet, dan de header er afslopen en verder
 * versturen naar afhandeling. Je zou dan eigenlijk voor elk verbinding een
 * MSGOBJECT objectje willen hebben die de data bewaart en vervolgens kopieert
 * naar een algemene buffer in geval van een compleet bericht en dan
 * uiteindelijk die versturen voor verdere verwerking. Zo een buffer is wel
 * een struct object met o.a. een veld wat aangeeft tot hoeveel de buffer
 * gevuld is. Dus bij elk type receive functie de bijbehorende buffer bewerken.
 * Dat had beter gekund, door een fd_obj object te voorzien van een buffer en
 * len variabele.
 *
 * We moeten ook wat switches (#defines) toevoegen voor compileer configuratie.
 * Bijv. instellen van grootte buffer voor inkomend socket verkeer.
 * By default gebruiken we 1 kB, maar we zouden ook 2 kB, 5 kB, 10 kB, 100 kB
 * of 1 MB gebruiken.
 *****************************************************************************/



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

#include <servercore/io_interface.h>
#include <servercore/log_writer.h>
#include <servercore/msg_handler.h>
#include <servercore/eth_sock.h>


//TODO: msgobject moet in fd_object zitten, zo heeft elk fd een eigen buffer.
static struct msgobject tcpbuff = {0};
static struct msgobject tempbuff = {0};
static int32_t client_fd = -1;
static void (*tcp_app_callback)(int32_t fd, char* data, int32_t len);
static char glbdata[MAX_BUF_SIZE];

extern void received_accept_from_client(int32_t fd);

int32_t recv_tcp_cb(struct fd_obj* fdo)
{
	int32_t rcvbytes = 0;

	DEBUG_MSG("Receiving data from client.");
	rcvbytes = recv(fdo->fd, glbdata, MAX_BUF_SIZE, 0);
	//rcvbytes = recv(fdo->fd, fdo->data, MAX_BUF_SIZE, 0);
	if(rcvbytes == 0)	{
		DEBUG_MSG("Client connection closed!");
		return -1;
	}
	else if(rcvbytes < 0) {
		DEBUG_MSG("Client error on receive!");
		return -1;
	} else {
		//appendbuffer(&tcpbuff,glbdata, rcvbytes);
		appendbuffer(fdo->data,glbdata, rcvbytes);
		memset(glbdata, 0x00, rcvbytes);
		while(getcompletemsg(&tcpbuff, &tempbuff))
			tcp_app_callback(fdo->fd, tempbuff.msg, tempbuff.len);

		memset(&tempbuff, 0x00, sizeof(struct msgobject));
	}

	return 0;
}


int32_t accept_new_client(struct fd_obj* fdo)
{
	int32_t client;
	struct fd_obj* new_fdo = NULL;
	struct sockaddr clientaddr = {0};
	int32_t len = sizeof(clientaddr);

	DEBUG_MSG("Receiving client request.");
	client = accept(fdo->fd, (struct sockaddr*) &clientaddr, (socklen_t*)&len);
	if(client < 0) {
		DEBUG_MSG("error on accepting client request.");
		perror("accept");
		return client;
	} else client_fd = client;

	DEBUG_MSG("Accepted a client connection succesfully.");
	new_fdo = add_fdo(client);
	if(new_fdo == NULL)	{
		DEBUG_MSG("Can't add fd's.");
	} else {
		new_fdo->fh = recv_tcp_cb;
	}

	DEBUG_MSG("Added client to the list.");

	return client;
}

int32_t init_tcp_server(int32_t portnr, char* ip,
                        void (*rcv_cb)(int32_t fd, char* data, int32_t len))
{
    int fd;
    struct fd_obj* new_fdo = NULL;

    fd = create_listener(ip, portnr, SOCK_STREAM, IPPROTO_TCP, 10);
    if(fd == -1){
        return fd;
    }

	tcp_app_callback = rcv_cb;
	new_fdo = add_fdo(fd);
	if(new_fdo == NULL)	{
		DEBUG_MSG("Can't add fd's.");
	} else {
		new_fdo->fh = accept_new_client;
	}

	DEBUG_MSG("Setup TCP server OK.");

	return 0;
}
