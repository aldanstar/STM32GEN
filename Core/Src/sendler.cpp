#include "sendler.h"
#include "tools.h" // Позже адаптируем этот файл под STM32

// Таблица CRC16 
const uint16_t sendler::crc16_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
    0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
    0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
    0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
    0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
    0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
    0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
    0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
    0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
    0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
    0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
    0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
    0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
    0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
    0xad2a, 0xb70b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
    0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
    0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
    0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
    0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
    0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
    0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
    0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
    0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
    0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
    0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
    0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
    0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
    0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xb99a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
    0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
    0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
    0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
    0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
    0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

sendler::sendler(UART_HandleTypeDef *huart) : huart(huart), rx_index(0), dataReceivedCallback(nullptr), header_received(false) {}

void sendler::begin(uint32_t baudrate, uint32_t config, uint8_t device_addr) {
    header.device_addr = device_addr;
    // Инициализация UART уже должна быть выполнена в main.c через MX_USART2_UART_Init()
    startReceive(); // Запускаем прием данных по прерыванию
}

void sendler::registerCallback(DataReceivedCallback callback) {
    dataReceivedCallback = callback;
}


uint16_t sendler::calc_crc16(const uint8_t *array, uint16_t len, uint16_t crc) {
    uint8_t temp;
    while (len--) {
        temp = ((crc >> 8) ^ *array++) & 0xFF;
        crc = crc16_table[temp] ^ (crc << 8);
    }
    return crc;
}

uint16_t sendler::setup(uint8_t dtype, uint16_t data_len) {
    header.frame_id = dtype;
    header.command_code = rx_header.command_code; // Используем последнее принятое значение command_code
    header.data_len = data_len;
    uint8_t buffer[sizeof(header_struct)];
    memcpy(buffer, &header, sizeof(header_struct));
    uint16_t header_crc = calc_crc16(buffer, sizeof(buffer), 0xFFFF);
    HAL_UART_Transmit(huart, buffer, sizeof(buffer), HAL_MAX_DELAY);
    return header_crc;
}

void sendler::send_empty() {
    uint16_t crc = this->setup(8, 0);
    uint8_t crc_array[2];
    tools::unpack_uint16(crc, crc_array);
    HAL_UART_Transmit(huart, crc_array, 2, HAL_MAX_DELAY);
}

void sendler::send_array(uint8_t arr[], uint16_t size) {
    uint16_t header_crc = this->setup(8, size);
    uint16_t crc = calc_crc16(arr, size, header_crc);
    uint8_t crc_array[2];
    tools::unpack_uint16(crc, crc_array);
    HAL_UART_Transmit(huart, arr, size, HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, crc_array, 2, HAL_MAX_DELAY);
}

void sendler::send_array(uint16_t arr[], uint16_t size) {
    uint16_t header_crc = this->setup(16, size);
    uint8_t arr_bit[size * 2];
    uint8_t buff[2];
    for (int i = 0; i < size; i++) {
        tools::unpack_uint16(arr[i], buff);
        memcpy(&arr_bit[i * 2], buff, sizeof(buff));
    }
    uint16_t crc = calc_crc16(arr_bit, sizeof(arr_bit), header_crc);
    uint8_t crc_array[2];
    tools::unpack_uint16(crc, crc_array);
    HAL_UART_Transmit(huart, arr_bit, sizeof(arr_bit), HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, crc_array, 2, HAL_MAX_DELAY);
}

void sendler::send_array(uint32_t arr[], uint16_t size) {
    uint16_t header_crc = this->setup(32, size);
    uint8_t arr_bit[size * 4];
    uint8_t buff[4];
    for (int i = 0; i < size; i++) {
        tools::unpack_uint32(arr[i], buff);
        memcpy(&arr_bit[i * 4], buff, sizeof(buff));
    }
    uint16_t crc = calc_crc16(arr_bit, sizeof(arr_bit), header_crc);
    uint8_t crc_array[2];
    tools::unpack_uint16(crc, crc_array);
    HAL_UART_Transmit(huart, arr_bit, sizeof(arr_bit), HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, crc_array, 2, HAL_MAX_DELAY);
}

void sendler::send_array(uint64_t arr[], uint16_t size) {
    uint16_t header_crc = this->setup(64, size);
    uint8_t arr_bit[size * 8];
    uint8_t buff[8];
    for (int i = 0; i < size; i++) {
        tools::unpack_uint64(arr[i], buff);
        memcpy(&arr_bit[i * 8], buff, sizeof(buff));
    }
    uint16_t crc = calc_crc16(arr_bit, sizeof(arr_bit), header_crc);
    uint8_t crc_array[2];
    tools::unpack_uint16(crc, crc_array);
    HAL_UART_Transmit(huart, arr_bit, sizeof(arr_bit), HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, crc_array, 2, HAL_MAX_DELAY);
}

void sendler::send_array_24(uint32_t arr[], uint16_t size) {
    uint16_t header_crc = this->setup(24, size);
    uint8_t arr_bit[size * 3];
    uint8_t buff[3];
    for (int i = 0; i < size; i++) {
        tools::unpack_uint32_as_24(arr[i], buff);
        memcpy(&arr_bit[i * 3], buff, sizeof(buff));
    }
    uint16_t crc = calc_crc16(arr_bit, sizeof(arr_bit), header_crc);
    uint8_t crc_array[2];
    tools::unpack_uint16(crc, crc_array);
    HAL_UART_Transmit(huart, arr_bit, sizeof(arr_bit), HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, crc_array, 2, HAL_MAX_DELAY);
}

void sendler::set_command_code(uint8_t code) {
    rx_header.command_code = code;
}

void sendler::startReceive() {
    header_received = false;
    rx_index = 0;
    HAL_UART_Receive_IT(huart, &rx_buffer[rx_index], 1); // Начинаем прием первого байта (адрес устройства)
}

void sendler::processReceivedData() {
    static uint16_t data_bytes_received = 0;
    uint16_t expected_data_len = rx_header.data_len;
    uint16_t expected_packet_size = sizeof(header_struct) + expected_data_len + 2; // Заголовок + данные + CRC

    if (!header_received) {
        if (rx_index == 0) {
            // memcpy(&rx_header.device_addr, rx_buffer, 1);
						memcpy((void*)&rx_header.device_addr, (const void*)rx_buffer, 1);
            if (rx_header.device_addr == header.device_addr) {
                HAL_UART_Receive_IT(huart, &rx_buffer[++rx_index], 1); // Принимаем command_code
            } else {
                startReceive(); // Не наш адрес, ждем снова
            }
        } else if (rx_index < sizeof(header_struct) -1 ) {
             HAL_UART_Receive_IT(huart, &rx_buffer[++rx_index], 1);
        } else if (rx_index == sizeof(header_struct) - 1) {
            // memcpy(&rx_header, rx_buffer, sizeof(header_struct));
						memcpy((void*)&rx_header, (const void*)rx_buffer, sizeof(header_struct));
            header_received = true;
            if (rx_header.data_len > 0) {
                HAL_UART_Receive_IT(huart, &rx_buffer[++rx_index], 1); // Начинаем прием данных
                data_bytes_received = 0;
            } else {
                // Нет данных, принимаем CRC
                 HAL_UART_Receive_IT(huart, &rx_buffer[++rx_index], 1);
            }
        }
    } else {
        if (rx_index < expected_packet_size -1) {
             HAL_UART_Receive_IT(huart, &rx_buffer[++rx_index], 1);
        } else if (rx_index == expected_packet_size - 1) {
            uint16_t received_crc;
            memcpy(&received_crc, &rx_buffer[rx_index - 1], 2); // Последние 2 байта - CRC

            uint8_t header_and_data[sizeof(header_struct) + rx_header.data_len];
            memcpy(header_and_data, rx_buffer, sizeof(header_struct) + rx_header.data_len);
            uint16_t calculated_crc = calc_crc16(header_and_data, sizeof(header_struct) + rx_header.data_len, 0xFFFF);

            if (calculated_crc == received_crc) {
                if (dataReceivedCallback != nullptr) {
                    uint8_t* data_values = nullptr;
                    if (rx_header.data_len > 0) {
                        data_values = (uint8_t*)malloc(rx_header.data_len);
                        memcpy(data_values, rx_buffer + sizeof(header_struct), rx_header.data_len);
                    }
                    dataReceivedCallback(rx_header.command_code, data_values, rx_header.data_len);
                }
            } else {
                // Ошибка CRC
            }
            startReceive(); // Готовимся к приему следующего сообщения
        }
    }
}

// Объявление глобального экземпляра sendler
extern UART_HandleTypeDef huart2;
sendler Sendler(&huart2);