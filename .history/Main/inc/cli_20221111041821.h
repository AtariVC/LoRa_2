#include "stm32l431xx.h"
#ifndef CODE_CLI_H_
#define CODE_CLI_H_

typedef enum Command{
    WAITING,
    ECHO,
    SINGLE_MEASURE,
    SET_TIME,
    SET_DATE,
    SET_RTC,
    GET_RTC,
    GET_MEASURED_DATA,
    SET_PIN_STATE,
    GET_PIN_STATE,
    SET_LORA_PARAMS,
    GET_LORA_PARAMS,
    SET_MEASURE_PERIOD,
    SET_SLEEP_PERIOD,
    START_MEASURE_ROUTINE,
    GET_RADIO_QUALITY,
    GET_BATTERY,
    MY_PERIODIC_DATA
} Command;

typedef struct CommandStruct{
    uint8_t control_word;
    uint8_t sender_id;
    uint8_t target_id;
    Command cmd;
    uint8_t arg_len;
    uint16_t args[32];
} CommandStruct;

void cli_parser(CommandStruct *data);

#endif