#include "adc.h"
#include "delay.h"

void ADC_Init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->CCIPR   &= ~RCC_CCIPR_ADCSEL;// set clock source from system clock

	ADC1->CR &= ~ADC_CR_DEEPPWD;  // exit from deep-p
	RCC->CCIPR   |=  ( 3 << RCC_CCIPR_ADCSEL_Pos );
	ADC1->CR |= ADC_CR_ADVREGEN;  // enable ADC voltage regulator
	Delay(20);

	ADC1->CR |= ADC_CR_ADCAL;
	while (!(ADC1->CR & ADC_CR_ADCAL));
	ADC1->CFGR &= ~ADC_CFGR_CONT;	// single run mode
	ADC1->CFGR |= ADC_CFGR_OVRMOD;  // overrun data register
	ADC1->CFGR |= ADC_CFGR_JQDIS;	// disable injected queue
	Delay(2);
	ADC1->CR |= ADC_CR_ADEN;

}
void ADC_Init_Channel(){
	//	ADC1->SMPR1 |= (7 << 12);
	ADC1->JSQR |= (4 << 8);
//	ADC1->JSQR |= 3; // JL = 0b0011: 4 conversions
//	ADC1->JSQR |= ADC_JSQR_JEXTSEL;
//	ADC1->JSQR |= ADC_JSQR_JEXTEN;
//	ADC1->CFGR |= ADC_CFGR_JAUTO;
//	ADC1->SMPR1 |= 2 << ADC_SMPR1_SMP4_Pos;
	ADC1_COMMON->CCR |= ADC_CCR_TSEN;
	ADC1->CR |= ADC_CR_JADSTART;
}

void ADC_Init_regular(uint8_t ch_num){
	// RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// VREFBUF->CSR &= ~VREFBUF_CSR_HIZ;
	// VREFBUF->CSR |= VREFBUF_CSR_ENVR;
	// while(!(VREFBUF->CSR & VREFBUF_CSR_VRR));

	RCC->AHB2ENR |=  ( RCC_AHB2ENR_ADCEN );
	RCC->CCIPR   &= ~( RCC_CCIPR_ADCSEL );
	RCC->CCIPR   |=  ( 3 << RCC_CCIPR_ADCSEL_Pos ); // set clock source from system clock


	// Bring the ADC out of 'deep power-down' mode.
	ADC1->CR    &= ~( ADC_CR_DEEPPWD );
	// Enable the ADC voltage regulator.
	ADC1->CR    |=  ( ADC_CR_ADVREGEN );
	// Delay for a handful of microseconds.
	Delay(100);
	// Calibrate the ADC if necessary.
	ADC1->CR  |=  ( ADC_CR_ADCAL );
	while ( ADC1->CR & ADC_CR_ADCAL ) {};
	if (ch_num <= 9) {
		ADC1->SMPR1 |= (7 << (ch_num * 3)); // set sample time to 2.5 cycles
	} else {
		ADC1->SMPR2 |= (7 << ((ch_num * 3) - 30));
	}
	ADC1->SMPR1 |= (7 << (6 * 3));
	ADC1->SMPR1 |= (7 << (7 * 3));
	ADC1->CFGR &= ~ADC_CFGR_CONT;	// single /run mode
	// ADC1->CFGR |= ADC_CFGR_CONT;
	ADC1->CFGR |= ADC_CFGR_OVRMOD;  // overrun data register
	ADC1->CFGR |= ADC_CFGR_JQDIS;	// disable injected queue !OBLIGATORY!
	// First, set the number of channels to read during each sequence.
	// (# of channels = L + 1, so set L to 0)
	// Configure the first (and only) step in the sequence to read channel 6.
	ADC1_COMMON->CCR |= ADC_CCR_VREFEN;

	ADC1->JSQR |= (ch_num << 8); // | (6 << 14) | (7 << 20) | (3 << 0);
	// ADC1->SQR1  &= ~( ADC_SQR1_L );
	// ADC1->SQR1  &= ~( 0x1F << 6 );
	// ADC1->SQR1  |=  ( channel_num << 6 );

	ADC1->CR |= ADC_CR_ADEN;

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
