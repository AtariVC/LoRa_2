#include "adc.h"
#include "delay.h"

void ADC_reset_registers(ADC_TypeDef* ADCx){
    ADCx->CR = 0x20000000;
    ADCx->CFGR = 0x80000000;  // reset value
    ADCx->IER = 0;
    ADCx->ISR = 0;
    ADCx->CFGR2 = 0;
    ADCx->SMPR1 = 0;
    ADCx->SMPR2 = 0;
    ADCx->TR1 = 0xFF0000;
    ADCx->TR2 = 0xFF0000;
    ADCx->TR3 = 0xFF0000;
    ADCx->SQR1 = 0;
    ADCx->SQR2 = 0;
    ADCx->SQR3 = 0;
    ADCx->SQR4 = 0;
    ADCx->JSQR = 0;
    ADCx->OFR1 = 0;
    ADCx->OFR2 = 0;
    ADCx->OFR3 = 0;
    ADCx->OFR4 = 0;
}

void ADC_Init(ADC *ADC_struct){
    ADC_reset_registers(ADC_struct->ADCx);
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;
	RCC->CCIPR |= 3 << RCC_CCIPR_ADCSEL_Pos;  // set clock source from system clock

	ADC_struct->ADCx->CR &= ~ADC_CR_DEEPPWD;  // Bring the ADC out of 'deep power-down' mode.
	ADC_struct->ADCx->CR |= ADC_CR_ADVREGEN;  // Enable the ADC voltage regulator.
	Delay(30);  // Delay for a handful of microseconds.
	ADC_struct->ADCx->CR |= ADC_CR_ADCAL;  // Calibrate the ADC.
	while (ADC_struct->ADCx->CR & ADC_CR_ADCAL);

    ADC_struct->ADCx->CFGR |= ADC_struct->mode << ADC_CFGR_CONT_Pos;  // continuous or single mode
    ADC_struct->ADCx->CFGR |= ADC_CFGR_OVRMOD;  // data will be always actual
    ADC_struct->ADCx->CFGR |= ADC_CFGR_JQDIS;  // Injected Queue disabled
    ADC_struct->ADCx->CFGR |= ADC_struct->trigger.polarity << ADC_CFGR_EXTEN_Pos;
    ADC_struct->ADCx->CFGR |= ADC_struct->trigger.exti_channel << ADC_CFGR_EXTSEL_Pos;
    ADC_struct->ADCx->CFGR |= ADC_struct->resolution << ADC_CFGR_RES_Pos;

    // enable internal channels
    if(ADC_struct->internal_channels.vref) ADC1_COMMON->CCR |= ADC_CCR_VREFEN;
    if(ADC_struct->internal_channels.vbat) ADC1_COMMON->CCR |= ADC_CCR_VBATEN;
    if(ADC_struct->internal_channels.temp) ADC1_COMMON->CCR |= ADC_CCR_TSEN;

    // enable oversampling
    if(ADC_struct->ovrsmpl_ratio != OVRSMPL_Disable){
        // ADC_struct->ADCx->CFGR2 |= ADC_CFGR2_JOVSE;
        ADC_struct->ADCx->CFGR2 |= ADC_CFGR2_ROVSE;  // Regular channels Oversampling Enable
        ADC_struct->ADCx->CFGR2 |= ADC_struct->ovrsmpl_ratio << ADC_CFGR2_OVSR_Pos;
        ADC_struct->ADCx->CFGR2 |= (ADC_struct->ovrsmpl_ratio + 1) << ADC_CFGR2_OVSS_Pos;
    }

    ADC_struct->vrefinternal_cal = *(uint16_t*)0x1FFF75AA;

    ADC_struct->ADCx->IER |= ADC_IER_EOCIE | ADC_IER_EOSIE;
    // ADC1->IER |= ADC_IER_JEOCIE;
    ADC_struct->ADCx->IER |= ADC_IER_JEOSIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);
}

void ADC_Channel_Init(uint8_t ch_num){
	if (ch_num <= 9) {
		ADC1->SMPR1 |= (7 << (ch_num * 3)); // set sample time to 2.5 cycles
	} else {
		ADC1->SMPR2 |= (7 << ((ch_num * 3) - 30));
	}
}

void ADC_Init_regular(uint8_t ch_num){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->CCIPR &= ~( RCC_CCIPR_ADCSEL );
	RCC->CCIPR |= 3 << RCC_CCIPR_ADCSEL_Pos; // set clock source from system clock

	ADC1->CR &= ~( ADC_CR_DEEPPWD );  // Bring the ADC out of 'deep power-down' mode.
	ADC1->CR |=  ( ADC_CR_ADVREGEN );  // Enable the ADC voltage regulator.
	// Delay for a handful of microseconds.
	Delay(30);

	ADC1->CR |= ADC_CR_ADCAL; // Calibrate the ADC if necessary.
	while ( ADC1->CR & ADC_CR_ADCAL );

    ADC_Channel_Init(ch_num);

	ADC1->CFGR &= ~ADC_CFGR_CONT;	// single /run mode
	// ADC1->CFGR |= ADC_CFGR_CONT;
	ADC1->CFGR |= ADC_CFGR_OVRMOD;  // overrun data register
	ADC1->CFGR |= ADC_CFGR_JQDIS;	// disable injected queue !OBLIGATORY!

	ADC1_COMMON->CCR |= ADC_CCR_VREFEN;

	ADC1->JSQR |= (ch_num << 8); // | (6 << 14) | (7 << 20) | (3 << 0);
	// ADC1->SQR1  &= ~( ADC_SQR1_L );
	// ADC1->SQR1  &= ~( 0x1F << 6 );
	// ADC1->SQR1  |=  ( channel_num << 6 );
    ADC1->IER |= ADC_IER_EOCIE;
    ADC1->IER |= ADC_IER_EOSIE;
    // ADC1->IER |= ADC_IER_JEOCIE;
    ADC1->IER |= ADC_IER_JEOSIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);
}

void ADC_Enable(ADC_TypeDef* ADCx){
    ADCx->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
    ADCx->ISR |= ADC_ISR_ADRDY;
}
void ADC_Disable(ADC_TypeDef* ADCx){

}

void adc_single_conversion( ADC_TypeDef* ADCx, uint16_t* adc_data_arr ) {
	// Start the ADC conversion.
	//   ADCx->CR  |=  ( ADC_CR_ADSTART );
	ADC1->CR |= ADC_CR_JADSTART;
	// Wait for the 'End Of Conversion' flag.
	Delay(50);
	//  while ( !( ADCx->ISR & ADC_ISR_JEOC ) ) {};
	// Read the converted value (this also clears the EOC flag).
	adc_data_arr[0] = ADCx->JDR1;
	// adc_data_arr[1] = ADCx->JDR2;
	// adc_data_arr[2] = ADCx->JDR3;

	// Wait for the 'End Of Sequence' flag and clear it.
	Delay(50);
	//  while ( !( ADCx->ISR & ADC_ISR_EOS ) ) {};
	ADCx->ISR |= ADC_ISR_JEOS;
	// ADCx->ISR |=  ( ADC_ISR_EOS );
}
