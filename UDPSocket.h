//
// Created by Jumail on 14/03/2017.
//

#ifndef UDP_RELIABLE_SOCKET_H
#define UDP_RELIABLE_SOCKET_H

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif

#define UDPSOCKET_ANYPORT 0
#define UDPSOCKET_INITBUFLEN 256
#define UDPSOCKET_RETRYTIMES 3
#define UDPSOCKET_RETRYSLEEPTIME 500000
WD
class UDPSocket {
public:
    UDPSocket(unsigned short port);
    ~UDPSocket();

    bool listen();
    bool connect(char *host, char *port);
    bool setBlockingMode(bool val);

    unsigned short getPort();

private:
    unsigned int sockfd;
    sockaddr_in address;

    unsigned short port;
    bool isBlocking;
};

bool InitializeSockets();
void ShutdownSockets();

#endif //UDP_RELIABLE_SOCKET_H
