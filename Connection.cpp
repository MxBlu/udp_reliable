//
// Created by Jumail on 17/03/2017.
//

#include "Connection.h"

Connection::Connection(int sockfd, sockaddr_in peer) {
    this->peer = peer;
    this->sockfd = sockfd;
}

Connection::~Connection() {

}
