#ifndef SOCKET_H
#define SOCKET_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>




class Socket
{
    public:
    Socket(int version);
    Socket();
    void Create_socket(int type, int protocol);
    void receive_msg();
    void close_socket();
    
    private:
    void Bind_socket();
    void Listen_for_connections();
    int sockfd;
    int newSock;
    int version {AF_INET};
    char buffer[1024];

};

#endif