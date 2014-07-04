#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"./shared/network.h"


void callBackFunc(Octet *buf,Uint32 len);
int pos=0;
char data[100000];
int main()
{
	char file[]="dest";
	char sourceFile[]="source";
	IPAddr ip=0xc0a832f5;
	printf("read source file%s\n",sourceFile);
	//tftp_get(ip,sourceFile,callBackFunc);
	for(pos=0;pos<1024;pos++)
		data[pos]='a';
	data[1023]='b';
	printf("begin write file to %s\n",file);
	tftp_put(ip,file,(Octet *)data,pos);
	printf("finish\n");
	return 0;
}


void callBackFunc(Octet *buf,Uint32 len)
{
	unsigned int i;
	for(i=0;i<len;i++,pos++)
		data[pos]=buf[i];
}

