#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H


#define MAX_BUF_SIZE 1024

struct msgheader {
	unsigned char beginmsg;
	u_int16_t size;
};

struct msgobject {
	int32_t len;
	char msg[MAX_BUF_SIZE];
	int32_t is_complete;
};


int32_t appendbuffer(struct msgobject* msgbuff, char* buffer, int32_t len);
int32_t getcompletemsg(struct msgobject* msgbuff, struct msgobject* helpbuff);
int32_t descendbuffer(struct msgobject* tempbuff, int32_t len);


#endif  //MSG_HANDLER_H
