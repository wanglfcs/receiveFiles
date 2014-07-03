#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"./shared/network.h"

#define udpPort 31500
// well-known server port
#define tftpPort 69

// tftp opcodes
#define tftpOpRRQ 1
#define tftpOpWRQ 2
#define tftpOpData 3
#define tftpOpAck 4
#define tftpOpError 5

// tftp packet layout
typedef struct TFTPHeader {
	short op;
	short block; // omitted for op=RRQ,WRQ; error number for op=Error
} TFTPHeader;
#define tftpPosName 2
#define tftpPosData 4

#define retryLimit 5    /* maximum number of transmission attempts */
#define timeout 3000000 /* receive timeout, in microseconds */


void sendData(IPAddr ip,char *data,int len);
void callBackFunc(Octet *buf,Uint32 len);
char *tftp_put(IPAddr server,char *file,Octet *data,Uint32 dataLen);
/*void appendStr(UDP *sendBuf, Uint32 *pos, char * str) {*/
	////Append null-terminated string to "sendBuf" at "pos", updating "pos"
	//int i = 0;
	//for (;;) {
		//sendBuf->data[*pos] = (Octet)str[i];
		//(*pos)++;
		//if (str[i] == 0) break;
		//i++;
	//}
/*}*/

void appendData(UDP *sendBuf,Uint32 start,Octet *data,Uint32 dataLen)
{
	Uint32 i;
	for(i=0;i<dataLen;i++)
	{
		sendBuf->data[start++]=data[i++];
	}
	for(;i<512;i++)
	{
		sendBuf->data[i]=0;
	}
}

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
	tftp_get(ip,file,callBackFunc);
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


char *tftp_put(IPAddr server,char *file,Octet *data,Uint32 dataLen)
{
	UDP *sendBuf=(UDP *)enet_alloc();
	printf("hello\n");
	UDPPort local=udp_allocPort(NULL);
	printf("hello\n");
	sendBuf->ip.dest=hton(server);
	printf("hello\n");
	sendBuf->udp.dest=htons(tftpPort);
	sendBuf->udp.srce=htons(local);
	printf("hello\n");
	TFTPHeader *sendHeader=(TFTPHeader *)&(sendBuf->data[0]);
	sendHeader->op=htons(tftpOpWRQ);
	Uint32 pos=tftpPosName;
	Uint32 dataStart;
	appendStr(sendBuf,&pos,file);
	appendStr(sendBuf,&pos,"octet");
	dataStart=pos;
	Uint32 blockNum;
	Uint32 sendLength;
	printf("begin send file\n");
	while(1)
	{
		Uint32 recvLen;
		IP *recvBuf;
		TFTPHeader *recvHeader;
		int tries;
		for(tries=0;;tries++)
		{
			if(tries>=retryLimit)
			{
				udp_freePort(local);
				enet_free((Enet *)sendBuf);
				return "Timeout";
			}
			udp_send(sendBuf,pos);
			recvLen=udp_recv(&recvBuf,local,timeout);
			if(recvLen)
			{
				recvHeader=(TFTPHeader *)udp_payload(recvBuf);
				if(ntohs(recvHeader->op)==tftpOpAck)
					break;
			}
		}
		//send block recvheader->block+1
		blockNum=recvHeader->block+1;
		sendHeader->op=htons(tftpOpData);
		sendHeader->block=htons(blockNum);
		sendLength=dataLen-512*(blockNum-1)>512?512:dataLen%512;
		appendData(sendBuf,dataStart,data+(blockNum-1)*512,sendLength);
		udp_recvDone(recvBuf);
		pos=dataStart+sendLength;
		if(sendLength<512)
		break;

	}
	udp_freePort(local);
	enet_free((Enet *)sendBuf);
	return NULL;
}
