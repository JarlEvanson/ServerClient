#ifndef GAME_SERVER
#define GAME_SERVER

#include "common/sockets.hpp"

class ClientDescriptor {
    Address clientAddress;
    uint64_t lastPacketTime;
    public:
        ClientDescriptor(void);
        ClientDescriptor(Address address, uint64_t time);
        uint64_t getLastPacketTime();
        Address getAddress();

        bool operator == (const ClientDescriptor& c);
};

#define SERVER_SOCKET_ADDRESS "127.0.0.1:2050"
#define SERVER_SOCKET_PORT 2050
#define WAIT_TIME_SECS 5

class Server {
    ClientDescriptor* clients;
    uint32_t maxConnections;
    uint32_t currentConnections;
    Socket socket;
    private:
        int32_t addConnection(Address address);
        void pruneConnections(void);
        int32_t checkConnection(Address from);
    public:
        Server(uint32_t maxConnections);
        void run(void);
        uint32_t getCurrentConnections(void);
};

#endif
