#include "serial/msg.hpp"


Msg::Msg() {
    data_ = nullptr;
    size_ = 0;
}


Msg::Msg(size_t data_size) : size_{Msg::SERVICE_BYTES + data_size} {
    data_ = new uint8_t[size_]{0};
}


Msg::Msg(size_t data_size, uint8_t* data) : Msg(data_size) {
    std::copy(data, data + size_, data_ + DATA_IDX);
}


Msg::Msg(size_t data_size, const std::initializer_list<uint8_t> array) : Msg(data_size) {
    std::copy(array.begin(), array.end(), data_ + DATA_IDX);
}


Msg::~Msg() {
    delete[] data_;
}


Msg::Msg(const Msg& other) : Msg(other.size_) {
    std::copy(other.data_, other.data_ + size_, data_);
}


Msg::Msg(Msg&& other) {
    size_ = other.size_;
    data_ = other.data_;
    other.data_ = nullptr;
}

Msg& Msg::operator=(const Msg& other) {
    if (this == &other) {
        return *this;
    }

    size_ = other.size_;

    delete[] data_;
    data_ = new uint8_t[size_];
    std::copy(other.data_, other.data_ + size_, data_);

    return *this;
}


Msg& Msg::operator=(Msg&& other) {
    if (this == &other) {
        return *this;
    }

    size_ = other.size_;

    delete[] data_;
    data_ = other.data_;
    other.data_ = nullptr;

    return *this;
}


uint8_t& Msg::operator[](size_t idx) {
    return data_[DATA_IDX + idx];
}


void Msg::setStartBytes() {
    if (size_ == 0) {
        return;
    }
    data_[START_BYTE_0_IDX] = START_BYTE;
    data_[START_BYTE_1_IDX] = START_BYTE;
}


void Msg::setChecksum(uint8_t checksum) {
    if (size_ == 0) {
        return;
    }
    data_[size_-1] = checksum;
}


uint8_t* Msg::storage() {
    return data_;
}


uint8_t* Msg::data() {
    return data_ + DATA_IDX;
}


size_t Msg::size() {
    return size_;
}


size_t Msg::data_size() {
    return size_ == 0 ? size_ : size_ - SERVICE_BYTES;
}


void Msg::print() {
    if (size_ == 0) {
        std::cout << "size = 0" << std::endl;
        return;
    }
    std::cout << "[";
    for (size_t i = 0; i < size_; i++) {
        std::cout << static_cast<int>(data_[i]);
        if (i != size_ - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}
