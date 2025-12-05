//Piotr Makulec, 2025
//Written for and tested on Arduino (ATmega328p).

#include <avr/io.h>
#include <avr/delay.h>

#include "lcd_display.h"

//measurement function
uint16_t measure(uint8_t canal);
void displayVoltage(uint16_t adcValue);

int main(void) {
  lcd_init();
  /*choosing the reference voltage
   * REFS0 - external from AREF pin
   * REFS0 | REFS1 - internal 1.1V
   */
   ADMUX |= (1<<REFS0);
   //ADC enable | prescaler
   ADCSRA |= (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0); //prescaler 64

   while(1) {
    lcd_cls(); //clear lcd
    
    displayVoltage((measure(5))); //measuring the voltage on ADC5 and displaying it
    _delay_ms(1000);
   }
}

uint16_t measure(uint8_t canal) {
  //set up corrent ADC canal
  ADMUX = (ADMUX & 0xF0) | canal;
  //start the measurement
  ADCSRA |= (1<<ADSC);
  //wait for the measurement to finish
  while(ADCSRA & (1<<ADSC));
  //return the 16-bit value
  return ADCW;
}

void displayVoltage(uint16_t adcValue) {
  //avoiding floats
  uint8_t int_part, dec_part;
  uint32_t result;
  result = adcValue * 49UL; //ADC = Vref/resolution * voltageDividerRatio (5V/1024 * 1)
  int_part = result / 10000; //separate integer part
  dec_part = (result / 100) % 100; //separate decimal part (2 decimal places)
  lcd_int(int_part);
  lcd_str(".");
  lcd_int(dec_part);
  lcd_str("V");
}
