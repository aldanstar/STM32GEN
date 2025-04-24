#include "tools.h"

void tools::unpack_uint16(uint16_t number, uint8_t *result) {
    // Упаковка uint16 в заранее поготовленный массив uint8 (Little-Endian)
    result[0] = number & 0xFF;
    result[1] = (number >> 8) & 0xFF;
}


uint16_t tools::pack_uint16(uint8_t *buffer) {
    // Распаковка из массива uint8 в uint16 и возращение числа (Little-Endian)
    uint16_t value = buffer[0];
    value |= (uint16_t)buffer[1] << 8;
    return value;
}

void tools::unpack_uint32(uint32_t number, uint8_t *result) {
    // Упаковка uint32 в заранее поготовленный массив uint8 (Little-Endian)
    result[0] = number & 0xFF; number >>= 8;
    result[1] = number & 0xFF; number >>= 8;
    result[2] = number & 0xFF; number >>= 8;
    result[3] = number & 0xFF;
}

uint32_t tools::pack_uint32(uint8_t *buffer) {
    // Распаковка из массива uint8 в uint32 и возращение числа (Little-Endian)
    uint32_t value = buffer[0];
    value |= (uint32_t)buffer[1] << 8;
    value |= (uint32_t)buffer[2] << 16;
    value |= (uint32_t)buffer[3] << 24;
    return value;
}


void tools::unpack_uint32_as_24(uint32_t number, uint8_t *result) {
    // Упаковка uint32 (где только 24 бита значащие) в заранее подготовленный массив uint8 из 3х байтов (Little-Endian)
    result[0] = number & 0xFF; number >>= 8;
    result[1] = number & 0xFF; number >>= 8;
    result[2] = number & 0xFF;
}


void tools::unpack_uint64(uint64_t number, uint8_t *result) {
    // Упаковка uint64 в заранее поготовленный массив uint8 (Little-Endian)
    result[0] = number & 0xFF; number >>= 8;
    result[1] = number & 0xFF; number >>= 8;
    result[2] = number & 0xFF; number >>= 8;
    result[3] = number & 0xFF; number >>= 8;
    result[4] = number & 0xFF; number >>= 8;
    result[5] = number & 0xFF; number >>= 8;
    result[6] = number & 0xFF; number >>= 8;
    result[7] = number & 0xFF;
}

uint64_t tools::pack_uint64(uint8_t *buffer) {
    // Распаковка из массива uint8 в uint64 и возращение числа (Little-Endian)
    uint64_t value = buffer[0];
    value |= (uint64_t)buffer[1] << 8;
    value |= (uint64_t)buffer[2] << 16;
    value |= (uint64_t)buffer[3] << 24;
    value |= (uint64_t)buffer[4] << 32;
    value |= (uint64_t)buffer[5] << 40;
    value |= (uint64_t)buffer[6] << 48;
    value |= (uint64_t)buffer[7] << 56;
    return value;
}