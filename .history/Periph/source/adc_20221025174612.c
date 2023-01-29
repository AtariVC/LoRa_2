#include "adc.h"
#include "delay.h"

void ADC_Init(ADC_struct *ADC){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;
	RCC->CCIPR |= 3 << RCC_CCIPR_ADCSEL_Pos; // set clock source from system clock

	ADC->ADC->CR &= ~ADC_CR_DEEPPWD;  // Bring the ADC out of 'deep power-down' mode.
	ADC->ADC->CR |= ADC_CR_ADVREGEN;  // Enable the ADC voltage regulator.
	// Delay for a handful of microseconds.
	Delay(30);

	ADC->ADC->CR |= ADC_CR_ADCAL; // Calibrate the ADC if necessary.
	while (ADC->ADC->CR & ADC_CR_ADCAL);

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

void ADC_Start(ADC_TypeDef* ADCx){
    ADCx->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
    ADCx->ISR |= ADC_ISR_ADRDY;
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
