#ifndef SENDLER_H
#define SENDLER_H

//#include <cstring>
//#include <cstdlib>


#include <string.h>  // Для memcpy, memset и других функций C
#include <stdlib.h>  // Для malloc/free

#include "main.h" // Включает stm32f4xx_hal.h и другие необходимые заголовочные файлы
#include <stdint.h>
#include <stdbool.h>


typedef void (*DataReceivedCallback)(uint8_t command_code, uint8_t* values, uint16_t data_len);

#pragma pack(1)
typedef struct {
    uint8_t device_addr;
    uint8_t command_code;
    uint8_t operation_id;
    uint8_t frame_id;
    uint16_t data_len;
} header_struct;
#pragma pack()

typedef struct {
    volatile bool available;
    uint8_t command_code;
    uint8_t *values;
    uint16_t data_len;
} data_struct;

class sendler {
public:
    sendler(UART_HandleTypeDef *huart);
    void begin(uint32_t baudrate, uint32_t config, uint8_t device_addr);
    void send_array(uint8_t arr[], uint16_t size);
    void send_array(uint16_t arr[], uint16_t size);
    void send_array(uint32_t arr[], uint16_t size);
    void send_array(uint64_t arr[], uint16_t size);
    void send_array_24(uint32_t arr[], uint16_t size);
    void send_empty();
    void set_command_code(uint8_t code);
    void registerCallback(DataReceivedCallback callback);

private:
    UART_HandleTypeDef *huart;
    uint8_t bits;
    uint8_t bytes_in_type;
    header_struct header;
    volatile header_struct rx_header;
    uint8_t rx_buffer[256]; // Пример размера приемного буфера
    volatile uint16_t rx_index;
    DataReceivedCallback dataReceivedCallback;
    volatile bool header_received;

    uint16_t setup(uint8_t dtype, uint16_t data_len);
    static const uint16_t crc16_table[256];
    uint16_t calc_crc16(const uint8_t *array, uint16_t len, uint16_t crc);
    void startReceive();
    void processReceivedData();
};

extern sendler Sendler; // Объявление глобального экземпляра
#endif