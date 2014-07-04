#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"./shared/network.h"

#define udpPort 31500

void sendData(IPAddr ip,char *data,int len);
void callBackFunc(Octet *buf,Uint32 len);
int pos=0;
char data[100000];
int main()
{
	char file[]="dest";
	char sourceFile[]="source";
	IPAddr ip=0xc0a832f5;
	printf("read source file%s\n",sourceFile);
	tftp_get(ip,sourceFile,callBackFunc);
	printf("begin write file to %s\n",file);
	tftp_put(ip,file,(Octet *)data,pos);
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
	unsigned int i;
	for(i=0;i<len;i++,pos++)
		data[pos]=buf[i];
}

