#! /usr/bin/python
import socket
address=('192.168.50.245',31500)
s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
s.bind(address)
while True:
    data,addr=s.recvfrom(2048)
    if not data:
        print "client has exist"
    print "received :",data,"from",addr
s.close()
