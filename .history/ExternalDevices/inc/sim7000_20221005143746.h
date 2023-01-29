

typedef struct GSM
{
    uint8_t rx;
    uint8_t tx;
    uint8_t rts;
    uint8_t cts;
    uint8_t ri;
    uint8_t dtr;
    uint8_t dcd;

    USART_TypeDef *uart;
};
