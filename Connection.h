//
// Created by Jumail on 17/03/2017.
//

#ifndef UDP_RELIABLE_CONNECTION_H
#define UDP_RELIABLE_CONNECTION_H

#include "UDPSocket.h"

class Connection {
public:
    Connection(int sockfd, sockaddr_in peer);
    ~Connection();

private:
    int sockfd;
    sockaddr_in peer;
};


#endif //UDP_RELIABLE_CONNECTION_H
