#include <iostream>
#include <string.h>
#include <assert.h>

#include "UDPSocket.h"

int main(int argc, char *argv[]) { // ./udp_reliable
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " [-s | -c] [host] [port]" << std::endl;
        return -1;
    }

    assert(InitializeSockets());

    if (strcmp(argv[1], "-c") == 0) {
        UDPSocket *client = new UDPSocket(UDPSOCKET_ANYPORT);
        std::cout << "Client port: " << client->getPort() << std::endl;
        std::cout << "Connection success: " << client->connect(argv[2], argv[3]) << std::endl;
        delete client;
    } else if (strcmp(argv[1], "-s") == 0) {
        UDPSocket *server = new UDPSocket((unsigned short) atoi(argv[2]));
        std::cout << "Server port: " << server->getPort() << std::endl;
        std::cout << "Connection success: " << server->listen() << std::endl;
        delete(server);
    } else {
        std::cerr << "Usage: " << argv[0] << " [-s | -c] [host] [port]" << std::endl;
        return -1;
    }

    ShutdownSockets();
    return 0;
}

