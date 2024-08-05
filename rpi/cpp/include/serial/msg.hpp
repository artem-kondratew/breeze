#ifndef SERIAL_MSG_HPP
#define SERIAL_MSG_HPP


#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>


class Msg {
public:
    static const uint8_t START_BYTE = 64;
    static const size_t SERVICE_BYTES = 3;
    static const size_t START_BYTE_0_IDX = 0;
    static const size_t START_BYTE_1_IDX = 1;
    static const size_t DATA_IDX = 2;

private:
    uint8_t* data_;
    size_t size_;

public:
    Msg();
    Msg(size_t data_size);
    Msg(size_t data_size, uint8_t* data);
    Msg(size_t data_size, const std::initializer_list<uint8_t> array);
    ~Msg();
    
    Msg(const Msg& other);
    Msg(Msg&& other);

    Msg& operator=(const Msg& other);
    Msg& operator=(Msg&& other);
    uint8_t& operator[](size_t idx);

    void setStartBytes();
    void setChecksum(uint8_t checksum);

    uint8_t* storage();
    uint8_t* data();
    size_t size();
    size_t data_size();

    void print();
};


#endif // SERIAL_MSG_HPP
