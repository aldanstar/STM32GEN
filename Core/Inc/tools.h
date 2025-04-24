#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

class tools
{
public:

    static void pack_array(uint8_t buffer[]);

    static void unpack_uint16(uint16_t number, uint8_t *result); // Упаковка uint16 в заранее поготовленный массив uint8
    static uint16_t pack_uint16(uint8_t *buffer); // Распаковка из массива uint8 в uint16 и возращение числа

    static void unpack_uint32(uint32_t number, uint8_t *result); // Упаковка uint32 в заранее поготовленный массив uint8
    static uint32_t pack_uint32(uint8_t *buffer); // Распаковка из массива uint8 в uint32 и возращение числа

    static void unpack_uint64(uint64_t number, uint8_t *result); // Упаковка uint64 в заранее поготовленный массив uint8
    static uint64_t pack_uint64(uint8_t *buffer); // Распаковка из массива uint8 в uint64 и возращение числа

    static void unpack_uint32_as_24(uint32_t number, uint8_t *result);

private:
    tools() {}
};

#endif