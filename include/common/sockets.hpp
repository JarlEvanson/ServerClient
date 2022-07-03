#ifndef GAME_SOCKETS
#define GAME_SOCKETS

#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC 2
#define PLATFORM_UNIX 3

#if defined(_WIN32) 
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS 
    #include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
#pragma comment(lib, "wsock32.lib")
#endif

class Address {
    private:
        sockaddr_in addr;
    public:
        Address(unsigned short port);
        Address(const char* string);
        Address(sockaddr_in address);
        int toIntAddress();
        int toPort();
        char* toString();
};

class Socket {
    private:
        SOCKET handle;
        bool initHandle();
    public:
        static bool initializeSockets();
        static void shutdownSockets();
        bool bindPort(Address address);
        bool Socket::sendTo(Address address, const char* packetData, int packetLen);
        int receivePacket(
            sockaddr_in* from,
            void* packetData, 
            unsigned int maxPacketSize
        );
        Socket();
        Socket(Address address);
        ~Socket();
};

#endif