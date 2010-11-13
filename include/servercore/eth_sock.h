#ifndef ETH_SOCK_H_INCLUDED
#define ETH_SOCK_H_INCLUDED


int32_t connect_socket(unsigned char * ipaddr, int32_t portnr, int32_t protocol);
int32_t send_to_client(int fd, char* data, int32_t len);
int32_t create_listener(char* ip, int port, int type, int protocol, int backlog);

#endif // ETH_SOCK_H_INCLUDED
