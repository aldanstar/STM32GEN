#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stdint.h>

void command_execute(uint8_t command_code, uint8_t* data, uint16_t data_len);

#endif