#ifndef CONFIG_SERIAL_H
#define CONFIG_SERIAL_H


#define CMD_SIZE        3 // Максимальный размер получаемых сообщений
#define MSG_SIZE        1 // Размер отправляемых сообщений

#define NOP             0 // heartbeat
#define LED_ON          1 // ВКЛ светодиод на D12
#define LED_OFF         2 // ВЫКЛ светодиод на D12
#define SET_VELOCITIES  3 // Установить значение скоростей на моторах
#define INIT_MOTORS     4 // Инициализация моторов

#define SET_COLOUR      5 // Установить основной цвет
#define SET_SECTOR      6 // Запись сектора
#define SET_ANGLE       7 // Получение угла сектора, вычисление и запись сектора

#endif