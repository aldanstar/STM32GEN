#include "command_handler.h"
#include "sendler.h"
#include "stdio.h" // Для printf (отладочный вывод)
#include "stdlib.h" // Для free

void command_execute(uint8_t command_code, uint8_t* data, uint16_t data_len) {
    printf("Command Code: %d, Data Length: %d\n", command_code, data_len);
    switch (command_code) {
        case 1: // STOP_CHECK (Пример кода команды)
            Sendler.send_empty();
            break;
        case 2: // START
            Sendler.send_empty();
            break;
        case 3: // SET_PARAMS
            Sendler.send_empty();
            break;
        case 4: // GET_PARAMS
            Sendler.send_empty();
            break;
        case 5: // START_ONCE_NEW_PARAMS
            Sendler.send_empty();
            break;
        case 6: // START_ONCE_OLD_PARAMS
            Sendler.send_empty();
            break;
        case 7: // GET_RESULT
            Sendler.send_empty();
            break;
        case 8: // GET_STATUS
            Sendler.send_empty();
            break;
        case 9: // DATA
            Sendler.send_empty();
            break;
        case 10: // SAVE_PARAMS
            Sendler.send_empty();
            break;
        default:
            break;
    }
    if (data != nullptr) {
        free(data); // Освобождаем память, выделенную под данные
    }
}