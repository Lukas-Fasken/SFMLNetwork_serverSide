#include "socket.h"

inline int z=InitializeWinsock();

Socket::Socket()
{

}

int Socket::Create_socket(int version, int type, int protocol)
{
    /*
    create socket
    input 
        version: IPv4->AF_INET
        type:   TCP -> SOCK_STREAM
                UDP -> SOCK_DGRAM
        protocol: 0
    */
    int sockfd=socket(version, type, protocol);
    if (sockfd<0)
    {
        std::cerr << "Socket creation failed!" << std::endl;
    }
    return sockfd;
}
void bind_socket()
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family;
    
}

