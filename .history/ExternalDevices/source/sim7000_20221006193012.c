#include "sim7000.h"
#include "gpio.h"

void SIM7000_Init(SIM7000 *driver){
    // gpio_init(driver->gpio.tx, )
}
void SIM7000_SendSMS(SIM7000 *driver, const char *data, const char *phone_num);
void SIM7000_SendTCP(SIM7000 *driver, const char *data, const char *ip, const char *port);