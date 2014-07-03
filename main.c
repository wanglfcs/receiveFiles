#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"./shared/network.h"

#define udpPort 31500

void sendData(IPAddr ip,char *data,int len);
void callBackFunc(Octet *buf,Uint32 len);

int main()
{
	char data[1000];
	char file[]="haha";
	int i;
	for(i=0;i<1000;i++)
		data[i]=i%26+'a';
	IPAddr ip=0xc0a832f5;
	printf("begin send file\n");
	//tftp_put(ip,file,(Octet *)data,1000);
	tftp_put(ip,file,callBackFunc);
	printf("finish\n");
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

void callBackFunc(Octet *buf,Uint32 len)
{
	printf("file length is %d conten is %s\n",len,(void *)buf);
}

