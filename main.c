#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"./shared/network.h"

#define udpPort 31500

void sendData(IPAddr ip,char *data,int len);

int main()
{
	char data[100];
	char hello[]="hello";
	int i;
	for(i=0;i<100;i++)
		data[i]=i+1;
	IPAddr ip=0xc0a832f5;
	for(i=0;i<20;i++)
	{
		//sendData(ip,data,100);
		sendData(ip,hello,sizeof(hello));
	}
	return 0;
}

void sendData(IPAddr ip,char *data,int len)
{
	UDP *sendBuff=(UDP *)enet_alloc();
	UDPPort local=udp_allocPort(NULL);
	sendBuff->ip.dest=hton(ip);
	sendBuff->udp.dest=htons(udpPort);
	sendBuff->udp.srce=htons(local);
	memcpy(sendBuff->data,data,len);
	udp_send(sendBuff,strlen(data));
	enet_free((Enet *)sendBuff);
}
