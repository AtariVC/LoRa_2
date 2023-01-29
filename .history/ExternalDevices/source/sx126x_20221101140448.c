#include "sx126x.h"
#include "spi.h"
#include "delay.h"

uint8_t *DUMMY_PTR = 0;

void SX126x_Init(SX126x *driver){
    gpio_init(driver->gpio.CS_pin, General_output, Push_pull, pull_up, Very_high_speed);
    gpio_init(driver->gpio.SCK_pin, driver->gpio.__SCK_AF_pin,  Push_pull, no_pull, Very_high_speed);
    gpio_init(driver->gpio.MISO_pin, driver->gpio.__MISO_AF_pin,  Open_drain, no_pull, Input);
    gpio_init(driver->gpio.MOSI_pin, driver->gpio.__MOSI_AF_pin,  Push_pull, no_pull, Very_high_speed);
    gpio_init(driver->gpio.reset_pin, General_output, Push_pull, pull_up, High_speed);
    gpio_init(driver->gpio.busy_pin, Input_mode, Open_drain, no_pull, Input);
    gpio_init(driver->gpio.DIO1_pin, Input_mode, Open_drain, no_pull, Input);
    gpio_state(driver->gpio.CS_pin, HIGH);
    gpio_state(driver->gpio.reset_pin, LOW);
    spi_init(driver->spi, div_2, Mode_0, data_8_bit, MSB);
    Delay(2);
    gpio_state(driver->gpio.reset_pin, HIGH);
    Delay(5);

    // SX126x_SetStandby(driver, 0);
    SX126x_SetDIO2AsRfSwitchCtr(driver, 1);
    SX126x_SetPacketType(driver, driver->packet_type);
    SX126x_GetPacketType(driver);
    SX126x_SetRfFrequency(driver, driver->frequency);
    SX126x_SetTxParams(driver, driver->power_dbm, driver->ramping_time);
    SX126x_SetModulationParams(driver, driver->spredingFactor, driver->bandWidth, driver->crcRate, driver->low_data_rate_optim);
    SX126x_GetDeviceErrors(driver);
    SX126x_SetDioIrqParams(driver, 1, 1, 0, 0);
    // SX126x_SetOCP(driver,  driver->overCurrentProtection);
    SX126x_SetSyncWord(driver, driver->sync_word);
}
void SX126x_SendData(SX126x *driver, uint8_t *data, uint8_t data_len){
    SX126x_SetBufferBaseAddress(driver, 0, 128);
    SX126x_SetPacketParams(driver, driver->preamble_len, driver->header_type, 15, driver->crc_on_off, driver->iq_polarity);
    SX126x_WriteBuffer(driver, data, data_len);
    SX126x_SetTx(driver, 0);
}

void SX126x_SendOpcode(SX126x *driver, uint8_t opcode, uint8_t *parameters, uint8_t param_len,
                       uint8_t *answer, uint8_t answer_len){
    while(gpio_read(driver->gpio.busy_pin));
    gpio_state(driver->gpio.CS_pin, LOW);
    driver->mode = spi_txrx(driver->spi, opcode);
    if(param_len > 0){
        for(uint8_t i = 0; i < param_len; i++)
            spi_txrx(driver->spi, parameters[i]);
    }
    if(answer_len > 0){
        for(uint8_t i = 0; i < answer_len; i++)
            answer[i] = (uint8_t)(spi_txrx(driver->spi, 0x00));
    }
    gpio_state(driver->gpio.CS_pin, HIGH);
}

void SX126x_GetStatus(SX126x *driver){
    SX126x_SendOpcode(driver, OPCODE_GET_STATUS, DUMMY_PTR, 0, &driver->mode, 1);
}

void SX126x_ReadRegisters(SX126x *driver, uint16_t address, uint8_t *answer, uint8_t len){
    SX126x_SendOpcode(driver, OPCODE_READ_REGISTER, (uint8_t *)(&address), 2, answer, len);
}

void SX126x_WriteRegisters(SX126x *driver, uint8_t *addr_and_regs, uint8_t len){
    SX126x_SendOpcode(driver, OPCODE_WRITE_REGISTER, addr_and_regs, len, DUMMY_PTR, 0);
}

void SX126x_WriteBuffer(SX126x *driver, uint8_t *offset_and_data, uint8_t len){
    SX126x_SendOpcode(driver, OPCODE_WRITE_BUFFER, offset_and_data, len, DUMMY_PTR, 0);
}
void SX126x_ReadBuffer(SX126x *driver, uint8_t offset, uint8_t *data, uint8_t len){
    SX126x_SendOpcode(driver, OPCODE_READ_BUFFER, &offset, 1, data, len);
}

void SX126x_SetRfFrequency(SX126x *driver, uint32_t freq_hz){
    uint32_t RF_Freq = (uint32_t)((float)(freq_hz / 32000000.0) * (1 << 25) );
    uint8_t data[4] = {RF_Freq >> 24, (RF_Freq >> 16) & 0xFF, (RF_Freq >> 8) & 0xFF, RF_Freq & 0xFF};
    uint8_t *reversed_data = (uint8_t *)(&RF_Freq);
    uint8_t d1 = reversed_data[0];
    uint8_t d2 = reversed_data[1];
    uint8_t d3 = reversed_data[2];
    uint8_t d4 = reversed_data[3];

    SX126x_SendOpcode(driver, OPCODE_SET_RF_FREQUENCY, data, 4, DUMMY_PTR, 0);
}

void SX126x_SetPacketType(SX126x *driver, uint8_t packet_type){
    SX126x_SendOpcode(driver, OPCODE_SET_PACKET_TYPE, &packet_type, 1, DUMMY_PTR, 0);
}

void SX126x_SetTxParams(SX126x *driver, uint8_t power_dbm, uint8_t ramp_time){
    uint8_t data[2] = {power_dbm, ramp_time};
    SX126x_SendOpcode(driver, OPCODE_SET_TX_PARAMS, data, 2, DUMMY_PTR, 0);
}

void SX126x_SetBufferBaseAddress(SX126x *driver, uint8_t tx_buf_addr, uint8_t rx_buf_addr){
    uint8_t data[2] = {tx_buf_addr, rx_buf_addr};
    SX126x_SendOpcode(driver, OPCODE_SET_BUFFER_BASE_ADDR, data, 2, DUMMY_PTR, 0);
}

void SX126x_SetModulationParams(SX126x *driver, uint8_t sf, uint8_t bw, uint8_t cr, uint8_t ldro){
    uint8_t data[8] = {sf, bw, cr, ldro, 0, 0, 0, 0};
    SX126x_SendOpcode(driver, OPCODE_SET_MODULATION_PARAMS, data, 8, DUMMY_PTR, 0);
}

void SX126x_SetPacketParams(SX126x *driver, uint16_t preamble_len, uint8_t header_type,
                            uint8_t payload_len, uint8_t crc_type, uint8_t invert_iq){
    uint8_t data[9] = {(uint8_t)(preamble_len >> 8), (uint8_t)(preamble_len & 0xFF), header_type,
                       payload_len, crc_type, invert_iq, 0, 0, 0};
    SX126x_SendOpcode(driver, OPCODE_SET_PACKET_PARAMS, data, 9, DUMMY_PTR, 0);
}

void SX126x_SetTx(SX126x *driver, uint16_t timeout_ms){
    // max timeout 262000 ms
    uint32_t timeout = timeout_ms / 0.015625;
    uint8_t data[3] = {timeout >> 16, (timeout >> 8) & 0xFF, timeout & 0xFF};
    SX126x_SendOpcode(driver, OPCODE_SET_TX, data, 3, DUMMY_PTR, 0);
}

void SX126x_SetRx(SX126x *driver, uint16_t timeout_ms){
    // max timeout 262000 ms ; 0xFFFFFF - continuous mode
    uint32_t timeout = timeout_ms / 0.015625;
    uint8_t data[3] = {timeout >> 16, (timeout >> 8) & 0xFF, timeout & 0xFF};
    SX126x_SendOpcode(driver, OPCODE_SET_RX, data, 3, DUMMY_PTR, 0);
}

void SX126x_SetStandby(SX126x *driver, uint8_t config){
    SX126x_SendOpcode(driver, OPCODE_SET_STANDBY, &config, 1, DUMMY_PTR, 0);
}

uint8_t SX126x_GetPacketType(SX126x *driver){
    uint8_t buf[2] = {0};
    SX126x_SendOpcode(driver, OPCODE_GET_PACKET_TYPE, DUMMY_PTR, 0, buf, 2);
    driver->mode = buf[0];
    driver->packet_type = buf[1];
    return buf[1];
}

uint16_t SX126x_GetDeviceErrors(SX126x *driver){
    uint8_t buf[3] = {0};
    SX126x_SendOpcode(driver, OPCODE_GET_DEVICE_ERRORS, DUMMY_PTR, 0, buf, 3);
    driver->mode = buf[0];
    return (uint16_t)((buf[1] << 8) | buf[2]);
}

void SX126x_SetOCP(SX126x *driver, uint8_t ocp){
    uint8_t data[3] = {0x08, 0xE7, ocp};
    SX126x_WriteRegisters(driver, data, 3);
}

void SX126x_SetDioIrqParams(SX126x *driver, uint16_t irq_mask, uint16_t dio1_mask, uint16_t dio2_mask, uint16_t dio3_mask){
    uint8_t data[8] = {irq_mask >> 8, irq_mask & 0xFF, dio1_mask >> 8, dio1_mask & 0xFF, dio2_mask >> 8, dio2_mask & 0xFF, dio3_mask >> 8, dio3_mask & 0xFF};
    SX126x_SendOpcode(driver, OPCODE_SET_DIO_IRQ_PARAMS, data, 8, DUMMY_PTR, 0);
}

void SX126x_SetDIO2AsRfSwitchCtr(SX126x *driver, uint8_t enable){
    SX126x_SendOpcode(driver, OPCODE_SET_DIO2_AS_RFSWITCH, &enable, 1, DUMMY_PTR, 0);
}

void SX126x_SetSyncWord(SX126x *driver, uint16_t sync_word){
    uint8_t data1[3] = {0x07, 0x40, (sync_word & 0xFF00) >> 8};
    uint8_t data2[3] = {0x07, 0x41, sync_word & 0xFF};
    SX126x_WriteRegisters(driver, data1, 3);
    SX126x_WriteRegisters(driver, data2, 3);
}