#ifndef SOCKET_H
#define SOCKET_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

// Call this function at the start of your program to initialize Winsock
inline int InitializeWinsock()
{
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
    {
        std::cerr << "WSA startup failed" << std::endl;
        return -1;
    }
    return 0;
}

class Socket
{
    public:
    Socket();
    int Create_socket(int version, int type, int protocol);

    private:
    

};

#endif