#ifndef GAME_SOCKETS
#define GAME_SOCKETS

#include "common/common.hpp"
#include "common/packet.hpp"

#if PLATFORM == PLATFORM_WINDOWS 
    #include <winsock2.h>
    #pragma comment(lib, "wsock32.lib")
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

class Packet;

class Address {
    private:
        sockaddr_in addr;
    public:
        Address(void);
        Address(unsigned short port);
        Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
        Address(const char* string);
        Address(sockaddr_in address);
        int toIntAddress();
        int toPort();
        char* toString();
        bool operator == (const Address& addr);
};

class Socket {
    private:
        int handle;
        bool initHandle();
    public:
        static bool initializeSockets();
        static void shutdownSockets();
        bool bindPort(Address address);
        bool sendTo(Address address, const char* packetData, int packetLen);
        bool sendTo(Address address, Packet& packet);
        int receivePacket(
            sockaddr_in* from,
            void* packetData, 
            unsigned int maxPacketSize
        );
        Socket();
        Socket(Address address);
        void move(Socket& source);
        ~Socket();
};

#endif