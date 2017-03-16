//
// Created by Jumail on 14/03/2017.
//

#define UDPSOCKET_HEADERHASH 0x8be6045b // CRC32 of UDP_RELIABLE

#include "UDPSocket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <thread>

UDPSocket::UDPSocket(unsigned short port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sockfd <= 0) {
        printf( "failed to create socket\n" );
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short) port);

    if (bind(sockfd, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
        printf( "failed to bind socket\n" );
        exit(1);
    }

    if (port == UDPSOCKET_ANYPORT) {
        sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);

        getsockname(sockfd, (sockaddr *)&addr, &addrLen);
        this->port = ntohs(addr.sin_port);
    } else
        this->port = port;

    isBlocking = false;
}

UDPSocket::~UDPSocket() {
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close(sockfd);
    #elif PLATFORM == PLATFORM_WINDOWS
    closesocket(sockfd);
    #endif
}

bool UDPSocket::setBlockingMode(bool val) {

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

    int nonBlocking = (int) val;
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
        printf( "failed to set non-blocking\n" );
        return false;
    }

    #elif PLATFORM == PLATFORM_WINDOWS

    DWORD nonBlocking = (DWORD) val;
    if (ioctlsocket(sockfd, FIONBIO, &nonBlocking) != 0) {
        printf( "failed to set non-blocking\n" );
        return false;
    }

    #endif
    isBlocking = val;
    return true;
}

unsigned short UDPSocket::getPort() {
    return this->port;
}

bool UDPSocket::listen() {
    unsigned char packet_data[UDPSOCKET_INITBUFLEN];

    sockaddr_in from;
    socklen_t fromLength = sizeof(from);

    int bytes = recvfrom(sockfd, (char*)packet_data, UDPSOCKET_INITBUFLEN, 0, (sockaddr*)&from, &fromLength);

    if (bytes <= 0) {
        fprintf(stderr, "UDPSocket::listen: no data received\n");
        return false;
    }

    if (*((int *) packet_data) == UDPSOCKET_HEADERHASH && packet_data[4] == 'c') {
        packet_data[4] = 'a';

        for (int i = 0; i < 5; i++) {
            if (sendto(sockfd, (char *)packet_data, 5, 0, (const sockaddr*) &from, fromLength) <= 0) {
                fprintf(stderr, "UDPSocket::listen: failed to send accept packet\n");
                return false;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(200000));
        }

        // TODO: Create Connection class

        return true;
    } else {
        fprintf(stderr, "UDPSocket::listen: incorrect header info\n");
        return false;
    }
}

bool UDPSocket::connect(char *host, char *port) {

    struct addrinfo hints, *addr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if (int rv = getaddrinfo(host, port, &hints, &addr) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }


    char packet[5];
    int hash = UDPSOCKET_HEADERHASH;
    char request = 'c';
    memcpy(&packet[0], &hash, 4);
    memcpy(&packet[4], &request, 1);

    char recvPacket[UDPSOCKET_INITBUFLEN];

    if(isBlocking) {
        for (int i = 0; i < UDPSOCKET_RETRYTIMES; i++) {
            int bytes = sendto(sockfd, (char *)packet, 5, 0, addr->ai_addr, addr->ai_addrlen);
            if (bytes <= 0) {
                freeaddrinfo(addr);
                fprintf(stderr, "UDPSocket::connect: packet failed to send\n");
                return false;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(UDPSOCKET_RETRYSLEEPTIME));

            bytes = recv(sockfd, recvPacket, UDPSOCKET_INITBUFLEN, 0);
            if (bytes <= 0)
                continue;

            if (*((int *) recvPacket) == UDPSOCKET_HEADERHASH && recvPacket[4] == 'a') {
                // TODO Create Connection class
                freeaddrinfo(addr);
                return true;
            }
        }
    } else {
        int bytes = sendto(sockfd, (char *)packet, 5, 0, addr->ai_addr, addr->ai_addrlen);

        if (bytes <= 0) {
            fprintf(stderr, "UDPSocket::connect: packet failed to send\n");
            freeaddrinfo(addr);
            return false;
        }

        bytes = recv(sockfd, recvPacket, UDPSOCKET_INITBUFLEN, 0);
        if (bytes <= 0) {
            fprintf(stderr, "UDPSocket::connect: no data received\n");
            return false;
        }

        if (*((int *) recvPacket) == UDPSOCKET_HEADERHASH && recvPacket[4] == 'a') {
            // TODO Create Connection class
            freeaddrinfo(addr);
            return true;
        }

    printf("UDPSocket::connect: connect timed out\n");
    return false;
}

bool InitializeSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
    #else
    return true;
    #endif
}

void ShutdownSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
    #endif
}