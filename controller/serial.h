#ifndef SERIAL_H
#define SERIAL_H


namespace serial {
    using callback = void (*) (uint8_t* data);
    
// service units
    constexpr uint64_t baudrate_ = 9600;
    constexpr uint8_t start_byte_ = 64;
    constexpr uint64_t service_bytes_ = 3;
    
    constexpr uint64_t start_byte_0_idx_ = 0;
    constexpr uint64_t start_byte_1_idx_ = 1;
    constexpr uint64_t data_idx_ = 2;    

    uint64_t cmd_size_;
    uint64_t msg_size_;

    uint64_t cmd_data_size_;
    uint64_t msg_data_size_;

// counter
    bool use_counter_;
    uint8_t counter_;
    uint64_t counter_timer_;
    uint64_t counter_delay_;

// callbacks
    callback connect_cb_;
    callback cb_;

// arrays with data
    uint8_t* cmd_;
    uint8_t* msg_;

// connection state field
    bool is_connected_;

// init function
    void init(uint64_t cmd_data_size, uint64_t msg_data_size, callback cb);

// checksum
    uint8_t crc8(uint8_t* data, int size);
    bool calcCheckSumForReceive();
    uint8_t calcCheckSumForSend(uint8_t* msg, uint64_t size);

// converters
    template<typename T>
    void num2arr(T number, uint8_t* output);
    template<typename T>
    T arr2num(uint8_t* data);

// user functions
    bool receive();
    void send(uint8_t* data);

    void connect();

    void spin();

    void ok();
    void nop(uint8_t* data) {}
    void nop() {}
    void enableCounter(uint64_t counter_delay);
}


void serial::init(uint64_t cmd_data_size, uint64_t msg_data_size, callback cb) {
    Serial.begin(baudrate_);
    Serial.setTimeout(0);

    cmd_data_size_ = cmd_data_size;
    msg_data_size_ = msg_data_size;

    cmd_size_ = service_bytes_ + cmd_data_size_;
    msg_size_ = service_bytes_ + msg_data_size_;

    cmd_ = new uint8_t[cmd_size_];
    msg_ = new uint8_t[msg_size_];
    
    connect_cb_ = [] (uint8_t* msg) {ok();};
    cb_ = cb;

    use_counter_ = false;
    counter_timer_ = millis();
}


uint8_t serial::crc8(uint8_t* data, int size) {
    uint8_t byte;
    uint8_t POLY = 0x7;
    uint8_t crc8 = 0xFF;

    for (int j = 0; j < size; j++) {

        byte = data[j];
        crc8 = crc8 ^ byte;

        for (int i = 0; i < 8; i++) {

            if (crc8 & 0x80) {
                crc8 = (crc8 << 1) ^ POLY;
            }
            else {
                crc8 = crc8 << 1;
            }
        }
    }
    return crc8;
}


bool serial::calcCheckSumForReceive() {
    return crc8(cmd_, cmd_size_) == 0;
}


uint8_t serial::calcCheckSumForSend(uint8_t* msg, uint64_t size) {
    return crc8(msg, size - 1);
}


template<typename T>
void serial::num2arr(T number, uint8_t* output) {
    memcpy(output, &number, sizeof(T));
}


template<typename T>
T serial::arr2num(uint8_t* data) {
    T number;
    memcpy(&number, data, sizeof(T));
    return number;
}


bool serial::receive() {
    if (Serial.available() >= cmd_size_) {
        uint8_t& b0 = cmd_[start_byte_0_idx_] = Serial.read();
        uint8_t& b1 = cmd_[start_byte_1_idx_] = Serial.read();
        if (b0 == start_byte_ && b1 == start_byte_) {
            for (uint64_t idx = data_idx_; idx < cmd_size_; idx++) {
                cmd_[idx] = Serial.read();
            }
            if (calcCheckSumForReceive()) {
                cb_(cmd_ + data_idx_);
                return true;
            }
        }
    }
    return false;
}


void serial::send(uint8_t* data) {
    memcpy(msg_ + data_idx_, data, msg_data_size_);
    msg_[start_byte_0_idx_] = start_byte_;
    msg_[start_byte_1_idx_] = start_byte_;
    msg_[msg_size_-1] = calcCheckSumForSend(msg_, msg_size_);
    Serial.write(msg_, msg_size_);
}


void serial::connect() {
    callback buf = cb_;
    cb_ = connect_cb_;
    while (true) {
        if (receive()) {
            cb_ = buf;
            return;
        }
        delay(100);
    }
}


void serial::spin() {
    if (use_counter_) {
        uint64_t new_time = millis();
        if (millis() - counter_timer_ < counter_delay_) {
            return;
        }
        static uint8_t* counter_data = new uint8_t[msg_data_size_];
        counter_data[msg_data_size_-1] = counter_;
        send(counter_data);
        counter_++;
        counter_timer_ = new_time;
    }
}


void serial::ok() {
    uint8_t data[msg_data_size_]{0};
    send(data);
}


void serial::enableCounter(uint64_t counter_delay) {
    use_counter_ = true;
    counter_delay_ = counter_delay;
}


#endif // SERIAL_H
