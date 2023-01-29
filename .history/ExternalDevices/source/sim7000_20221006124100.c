#include "sim7000.h"

void SIM7000_Init(SIM7000 *driver){
    driver->gpio = SIM7000_GPIO gpio();
}
void SIM7000_SendSMS(SIM7000 *driver, const char *data, const char *phone_num);
void SIM7000_SendTCP(SIM7000 *driver, const char *data, const char *ip, const char *port);