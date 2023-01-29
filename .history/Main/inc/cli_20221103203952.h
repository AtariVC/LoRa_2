#include "stm32l431xx.h"

typedef struct CommandStruct{
    uint8_t control_word;
    uint8_t sender_id;
    uint8_t target_id;
    Command cmd;
    uint8_t arg_len;
    uint8_t *args;
} CommandStruct;

typedef enum Command{
    ECHO,
    SET_RTC,
    GET_RTC,
    START_MEASURE,
    GET_DATA,
    SET_PIN_STATE,
    SET_LORA_PARAMS,
    SET_MEASURE_PERIOD,
    SET_SLEEP_PERIOD
} Command;

void cli_parser(CommandStruct *data);