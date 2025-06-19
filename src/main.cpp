#include "Sockets/socket.h"
#include <winsock2.h>
#include <ws2tcpip.h>
int main()
{
    std::cout << "1" << std::endl;
    #ifdef _WIN32
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
    {
        std::cerr << "WSA startup failed" << std::endl;
    }
    #endif
    std::cout << "2" << std::endl;
    Socket socket;
    std::cout << "3" << std::endl;
    socket.Create_socket(SOCK_STREAM, 0);
    std::cout << "4" << std::endl;
    socket.receive_msg();
    system("pause");
    socket.close_socket();
}