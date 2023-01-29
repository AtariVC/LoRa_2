#include "main.h"
#include "cli.h"
#include "sx126x.h"


void cli_parser(CommandStruct *data){
    switch (data->cmd)
    {
    case ECHO:
        SX126x_SendData(&SX1268);
        break;
    case SET_RTC:

        break;
    case GET_RTC:

        break;
    case START_MEASURE:

        break;
    case GET_DATA:

        break;
    case SET_PIN_STATE:

        break;
    case SET_LORA_PARAMS:
        break;
    case SET_MEASURE_PERIOD:
        break;
    case SET_SLEEP_PERIOD:

        break;
    default:
        break;
    }
}
