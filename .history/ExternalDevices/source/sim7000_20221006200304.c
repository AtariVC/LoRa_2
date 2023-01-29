#include "sim7000.h"
#include "gpio.h"

void SIM7000_Init(SIM7000 *driver){
    gpio_init(driver->gpio.tx, driver->gpio.__tx_af_pin, Push_pull, no_pull, Medium_speed);
    gpio_init(driver->gpio.rx, driver->gpio.__rx_af_pin, Push_pull, no_pull, Medium_speed);
    gpio_init(driver->gpio.pwr, General_output, Push_pull, pull_up, Low_speed);
    gpio_init(driver->gpio.cts, driver->gpio.)
}
void SIM7000_SendSMS(SIM7000 *driver, const char *data, const char *phone_num);
void SIM7000_SendTCP(SIM7000 *driver, const char *data, const char *ip, const char *port);