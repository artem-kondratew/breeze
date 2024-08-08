#ifndef SOCKET_HPP
#define SOCKET_HPP


#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <iostream>
#include <vector>


class Socket {
private:
    int fd_;
    size_t port_;
    sockaddr_in server_address_;
    uint8_t* buffer_;
    size_t buffer_size_;
    bool verbose_;

public:
    Socket(size_t port, size_t buffer_size);
    Socket(const Socket& other) = delete;
    Socket(Socket&& other) = delete;
    ~Socket();

    Socket* operator=(const Socket& other) = delete;
    Socket* operator=(Socket&& other) = delete;

    std::vector<uint8_t> read();

    void enableVerbose();
    void disableVerbose();
};


#endif // SOCKET_HPP
