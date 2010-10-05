#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <servercore/msg_handler.h>
#include <servercore/log_writer.h>

//Eerst de binnengekomen data in de buffer aanvullen
int32_t appendbuffer(struct msgobject* msgbuff, char* buffer, int32_t len)
{
	//Check eerst op risico van buffer overflow!
	if(msgbuff->len+len > MAX_BUF_SIZE)
		return 0;

	memcpy(&msgbuff->msg[msgbuff->len], buffer, len);
	msgbuff->len += len;

	return len;
}

//vervolgens zoeken naar een compleet bericht en die eruit halen.
int32_t getcompletemsg(struct msgobject* msgbuff, struct msgobject* helpbuff)
{
	u_int16_t len = 0;

	if(msgbuff->msg[0] != 0x00) {
		DEBUG_MSG("Wrong begin message.");
		return 0;
	}

	//len is dus het uieindelijke bericht: TLV zonder header.
	len = msgbuff->msg[1]|(msgbuff->msg[2]<<8);
	if(msgbuff->len < len)
		return 0;	//len klopt niet, dit zou niet mogen gebeuren

	memcpy(helpbuff->msg, msgbuff->msg+3, len);	//-3 is minus de header data
	helpbuff->len = len;
	helpbuff->is_complete = 1;

	descendbuffer(msgbuff, len+3);	//+3: plus header lengte

	return 1;
}

int32_t descendbuffer(struct msgobject* msgobj, int32_t len)
{
	int32_t newlen = msgobj->len - len;

	memcpy(msgobj->msg, &msgobj->msg[len], newlen);
	msgobj->len = newlen;
	memset(&msgobj->msg[msgobj->len], 0x00, len);

	return 0;
}


