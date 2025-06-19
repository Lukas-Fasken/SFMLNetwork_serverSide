#include "socket.h"

Socket::Socket(int version) : version{version}
{
    /*
    version: IPv4->AF_INET
    */
}
Socket::Socket()
{

}

void Socket::Create_socket(int type, int protocol)
{
    /*
    create socket
    input 
        type:   TCP -> SOCK_STREAM
                UDP -> SOCK_DGRAM
        protocol: 0
    */
    sockfd=socket(version, type, protocol);
    if (sockfd<0)
    {
        std::cerr << "Socket creation failed!" << std::endl;
    }
    Bind_socket();
    Listen_for_connections();
}
void Socket::Bind_socket()
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family=version;
    serverAddr.sin_port= htons(8080);
    serverAddr.sin_addr.s_addr=INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed!" << std::endl;
    }
}

void Socket::Listen_for_connections()
{
    /*
    add timeout
    */
    std::cout << "listening" << std::endl;
    struct sockaddr_in clientAddr;
    while (listen(sockfd, 5)<1)// 5: number of allowed connections
    {

    }
    std::cout << "connected" << std::endl;
    socklen_t clientLen=sizeof(clientAddr);

    newSock = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
    if (newSock<0)
    {
        std::cerr << "Accept failed!" << std::endl;
    }
}

void Socket::receive_msg()
{
    recv(newSock, buffer, sizeof(buffer), 0);
    std::cout << "Received: " << buffer << std::endl;
}

void Socket::close_socket()
{
    closesocket(sockfd);
    WSACleanup();
}