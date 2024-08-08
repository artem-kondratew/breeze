#include "serial/socket.hpp"


Socket::Socket(size_t port, size_t buffer_size) : port_{port}, buffer_size_{buffer_size} {
    if ((fd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("socket creation failed"); 
        exit(1); 
    }

    buffer_ = new uint8_t[buffer_size_]{0};

    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(port_);
    server_address_.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd_, (const struct sockaddr*)&server_address_, sizeof(server_address_)) < 0) { 
        perror("bind failed"); 
        exit(2); 
    }

    verbose_ = true;
}


Socket::~Socket() {
    delete[] buffer_;
}


std::vector<uint8_t> Socket::read() {
    int n = recvfrom(fd_, static_cast<uint8_t*>(buffer_), buffer_size_, MSG_WAITALL, nullptr, nullptr);
    if (n == -1) {
        if (verbose_) {
            std::cout << "failed to read message" << std::endl;
        }
        return std::vector<uint8_t>();
    }

    if (verbose_) {
        std::cout << "[";
        for (size_t i = 0; i < buffer_size_; i++) {
            std::cout << static_cast<int>(buffer_[i]);
            if (i != buffer_size_ - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
    
    std::vector<uint8_t> msg;
    msg.resize(buffer_size_);
    std::copy(buffer_, buffer_ + buffer_size_, msg.data());
    return msg;
}


void Socket::enableVerbose() {
    verbose_ = true;
}


void Socket::disableVerbose() {
    verbose_ = false;
}
