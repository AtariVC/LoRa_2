#include "adc.h"
#include "delay.h"

void ADC_Init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->CCIPR   &= ~( RCC_CCIPR_ADCSEL );
	RCC->CCIPR   |=  ( 3 << RCC_CCIPR_ADCSEL_Pos );
	ADC1_COMMON->CCR |= (4 << 18); //devide by 8

	ADC1->CR &= ~ADC_CR_DEEPPWD;
	ADC1->CR |= ADC_CR_ADVREGEN;
	Delay(100);

	ADC1->CR |= ADC_CR_ADCAL;
	while (!(ADC1->CR & ADC_CR_ADCAL));
//	ADC1->SMPR1 |= (7 << 12);
	ADC1->JSQR |= (4 << 8);
//	ADC1->JSQR |= 3; // JL = 0b0011: 4 conversions
//	ADC1->JSQR |= ADC_JSQR_JEXTSEL;
//	ADC1->JSQR |= ADC_JSQR_JEXTEN;
	ADC1->CFGR &= ~ADC_CFGR_CONT;
//	ADC1->CFGR |= ADC_CFGR_JAUTO;
	ADC1->CFGR |= ADC_CFGR_JQDIS;
	ADC1->CFGR |= ADC_CFGR_OVRMOD;
//	ADC1->SMPR1 |= 2 << ADC_SMPR1_SMP4_Pos;
	Delay(20);
	ADC1->CR |= ADC_CR_ADEN;
	ADC1->CR |= ADC_CR_JADSTART;
}
void ADC_Init_regular(){
	RCC->AHB2ENR |=  ( RCC_AHB2ENR_ADCEN );
	RCC->CCIPR   &= ~( RCC_CCIPR_ADCSEL );
	RCC->CCIPR   |=  ( 3 << RCC_CCIPR_ADCSEL_Pos );
	ADC1_COMMON->CCR |= (4 << 18); //devide by 8
	// Bring the ADC out of 'deep power-down' mode.
	ADC1->CR    &= ~( ADC_CR_DEEPPWD );
	// Enable the ADC voltage regulator.
	ADC1->CR    |=  ( ADC_CR_ADVREGEN );
	// Delay for a handful of microseconds.
	Delay(100);
	// Calibrate the ADC if necessary.
	ADC1->CR  |=  ( ADC_CR_ADCAL );
	while ( ADC1->CR & ADC_CR_ADCAL ) {};
	// First, set the number of channels to read during each sequence.
	// (# of channels = L + 1, so set L to 0)
	ADC1->SQR1  &= ~( ADC_SQR1_L );
	// Configure the first (and only) step in the sequence to read channel 6.
	ADC1->SQR1  &= ~( 0x1F << 6 );
	ADC1->SQR1  |=  ( 4 << 6 );
	// Configure the sampling time to 640.5 cycles.
//	ADC1->SMPR1 |=  ( 0x2 << 12 );
}

uint16_t adc_single_conversion( ADC_TypeDef* ADCx ) {
  // Start the ADC conversion.
  ADCx->CR  |=  ( ADC_CR_ADSTART );
  // Wait for the 'End Of Conversion' flag.
  Delay(50);
//  while ( !( ADCx->ISR & ADC_ISR_EOC ) ) {};
  // Read the converted value (this also clears the EOC flag).
  uint16_t adc_val = ADCx->DR;
  // Wait for the 'End Of Sequence' flag and clear it.
  Delay(50);
//  while ( !( ADCx->ISR & ADC_ISR_EOS ) ) {};
  ADCx->ISR |=  ( ADC_ISR_EOS );
  // Return the ADC value.
  return adc_val;
}
