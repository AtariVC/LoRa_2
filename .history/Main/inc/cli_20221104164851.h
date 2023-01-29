#include "stm32l431xx.h"
#ifndef CODE_CLI_H_
#define CODE_CLI_H_

typedef enum Command{
    WAITING,
    ECHO,
    SET_RTC,
    GET_RTC,
    START_MEASURE,
    GET_DATA,
    SET_PIN_STATE,
    SET_LORA_PARAMS,
    SET_MEASURE_PERIOD,
    SET_SLEEP_PERIOD,
    START_MEASURE_ROUTINE
} Command;

typedef struct CommandStruct{
    uint8_t control_word;
    uint8_t sender_id;
    uint8_t target_id;
    Command cmd;
    uint8_t arg_len;
    uint8_t *args;
} CommandStruct;

void cli_parser(CommandStruct *data);

#endif