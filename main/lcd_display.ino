//Piotr Makulec, 2025
//Written for and tested on Arduino (ATmega328p).

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>

#include "lcd_display.h"

//macrodefiniditions - RS, RW and E pin control signals
#define SET_RS PORT(LCD_RSPORT) |= (1<<LCD_RS)    //set RS HIGH
#define CLR_RS PORT(LCD_RSPORT) &= ~(1<<LCD_RS)   //set RS LOW
#define SET_RW PORT(LCD_RWPORT) |= (1<<LCD_RW)    //set RW HIGH
#define CLR_RW PORT(LCD_RWPORT) &= ~(1<<LCD_RW)   //set RW LOW
#define SET_E PORT(LCD_EPORT) |= (1<<LCD_E)      //set E HIGH
#define CLR_E PORT(LCD_EPORT) &= ~(1<<LCD_E)      //set E LOW

uint8_t check_BF(void);

//--------------------------------------------------------------------

//INTERNAL FUNCTIONS

//set all 4 data lines as outputs
static inline void data_dir_out() {
  DDR(LCD_D7PORT) |= (1 << LCD_D7);
  DDR(LCD_D6PORT) |= (1 << LCD_D6);
  DDR(LCD_D5PORT) |= (1 << LCD_D5);
  DDR(LCD_D4PORT) |= (1 << LCD_D4);
}

//set all 4 data lines as inputs
static inline void data_dir_in() {
  DDR(LCD_D7PORT) &= ~(1 << LCD_D7);
  DDR(LCD_D6PORT) &= ~(1 << LCD_D6);
  DDR(LCD_D5PORT) &= ~(1 << LCD_D5);
  DDR(LCD_D4PORT) &= ~(1 << LCD_D4);
}

//send half of the bite to the LCD
static inline void lcd_sendHalf(uint8_t data) {
  if (data & (1 << 0)) PORT(LCD_D4PORT) |= (1 << LCD_D4);
  else PORT(LCD_D4PORT) &= ~(1 << LCD_D4);

  if (data & (1 << 1)) PORT(LCD_D5PORT) |= (1 << LCD_D5);
  else PORT(LCD_D5PORT) &= ~(1 << LCD_D5);

  if (data & (1 << 2)) PORT(LCD_D6PORT) |= (1 << LCD_D6);
  else PORT(LCD_D6PORT) &= ~(1 << LCD_D6);

  if (data & (1 << 3)) PORT(LCD_D7PORT) |= (1 << LCD_D7);
  else PORT(LCD_D7PORT) &= ~(1 << LCD_D7);
}

#if USE_RW == 1
//read half of the bite from LCD
static inline uint8_t lcd_readHalf(void) {
  uint8_t result = 0;
  if (PIN(LCD_D4PORT) & (1 << LCD_D4)) result |= (1 << 0);
  if (PIN(LCD_D5PORT) & (1 << LCD_D5)) result |= (1 << 1);
  if (PIN(LCD_D6PORT) & (1 << LCD_D6)) result |= (1 << 2);
  if (PIN(LCD_D7PORT) & (1 << LCD_D7)) result |= (1 << 3);
  return result;
}
#endif

//write bite to the LCD
void _lcd_write_byte(unsigned char _data) {
  data_dir_out();

#if USE_RW == 1
  CLR_RW;
#endif
  SET_E;
  lcd_sendHalf(_data >> 4);
  CLR_E;

  SET_E;
  lcd_sendHalf(_data);
  CLR_E;

#if USE_RW == 1
  while ((check_BF() & (1 << 7))); //wait for LCD Busy Flag
#else
  _delay_us(120);
#endif
}

#if USE_RW == 1
//read bite from LCD
uint8_t _lcd_read_byte(void) {
  uint8_t result = 0;
  data_dir_in();

  SET_RW;
  SET_E;
  result |= (lcd_readHalf() << 4);  //read D7...D4
  CLR_E;
  SET_E;
  result |= lcd_readHalf();         //read D3...D0
  CLR_E;

  return result;
}
#endif

#if USE_RW == 1
//check BF - Busy Flag (whether LCD is busy)
uint8_t check_BF(void) {
  CLR_RS;
  return _lcd_read_byte();
}
#endif

//write a command to LCD
void lcd_write_cmd(uint8_t cmd) {
  CLR_RS;
  _lcd_write_byte(cmd);
}

//write data to LCD
void lcd_write_data(uint8_t data) {
  SET_RS;
  _lcd_write_byte(data);
}

//-------------------------------------------------

//FUNCTIONS TO INTERACT WITH MODULES

#if USE_LCD_CHAR == 1

/* display strings saved in RAM
   while the character is not "\0"
*/
void lcd_str(char * str) {
  register char x;
  while ((x = *(str++))) lcd_write_data(x);
}

#if USE_LCD_STR_P == 1
/* display strings saved in FLASH
   while the character is not "\0"
*/
void lcd_str_P(const char* str) {
  register char x;
  while ((x = pgm_read_byte(str++))) lcd_write_data(x);
}
#endif

#if USE_LCD_STR_E == 1
/* display strings saved in EEPROM
   while the character is not "\0"
*/
void lcd_str_E(char* str) {
  register char x;
  while (1) {
    x = eeprom_read_byte((uint8_t *)(str++));
    if (!x || x == 0xFF) break;               //0xFF considered as 0 in EEPROM
    else lcd_write_data(x);
  }
}
#endif
#endif

#if USE_LCD_INT == 1
/*  display an integer (decimal)
     convert it to the string first
*/
void lcd_int(int val) {
  char buf[17];
  lcd_str(itoa(val, buf, 10));
}
#endif

#if USE_LCD_HEX == 1
/*  display an integer (hexidecimal)
     convert it to the string first
*/
void lcd_hex(int val) {
  char buf[17];
  lcd_str(itoa(val, buf, 16));
}
#endif

#if USE_LCD_DEFCHAR == 1
/*  custom character definition on the LCD from RAM
    agruments:
    nr: character code - 0x80 to 0x87
 *  * def_char - a pointer to the table of 7 bites defining the charakter
*/
void lcd_defchar(uint8_t nr, uint8_t * def_char) {
  register uint8_t i, c;
  lcd_write_cmd(64 + (nr * 8));
  for (i = 0; i < 8; i++) {
    c = *(def_char++);
    lcd_write_data(c);
  }
}
#endif

#if USE_LCD_DEFCHAR_P == 1
/*  custom character definition on the LCD from FLASH
    agruments:
    nr: character code - 0x80 to 0x87
 *  * def_char - a pointer to the table of 7 bites defining the charakter
*/
void lcd_defchar_P(uint8_t nr, const uint8_t * def_char) {
  register uint8_t i, c;
  lcd_write_cmd(64 + (nr * 8));
  for (i = 0; i < 8; i++) {
    c = pgm_read_byte(def_char++);
    lcd_write_data(c);
  }
}
#endif

#if USE_LCD_DEFCHAR_E == 1
/*  custom character definition on the LCD from EEPROM
    agruments:
    nr: character code - 0x80 to 0x87
 *  * def_char - a pointer to the table of 7 bites defining the character
*/
void lcd_defchar_E(uint8_t nr, uint8_t * def_char) {
  register uint8_t i, c;
  lcd_write_cmd(64 + (nr * 8));
  for (i = 0; i < 8; i++) {
    c = eeprom_read_byte(def_char++);
    lcd_write_data(c);
  }
}
#endif

#if USE_LCD_LOCATE == 1
/* set the cursor in position (Y-row, X-column)
   Y = <0; 3>; X = <0; n>
   function automatically sets the DDRAM addresses depending on diplay's size
*/
void lcd_locate(uint8_t y, uint8_t x) {
  switch (y) {
    case 0:
      y = LCD_LINE1;
      break;
#if (LCD_Y > 1)
    case 1:
      y = LCD_LINE2;
      break;
#endif
#if (LCD_Y > 2)
    case 2:
      y = LCD_LINE3;
      break;
#endif
#if (LCD_Y > 3)
    case 3:
      y = LCD_LINE4;
      break;
#endif
  }
  lcd_write_cmd((0x80 + y + x));
}
#endif

//clear the display
void lcd_cls(void) {
  lcd_write_cmd(LCDC_CLS);
#if USE_RW == 0
  _delay_ms(4.9);
#endif
}

#if USE_LCD_CURSOR_HOME == 1
//moving the cursor to the start
void lcd_home(void) {
  lcd_write_cmd(LCDC_CLS | LCDC_HOME);

#if USE_RW == 0
  _delay_ms(4.9);
#endif
}
#endif

#if USE_LCD_CURSOR_ON == 1
//turn on the cursor
void lcd_cursor_on(void) {
  lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAY_ON | LCDC_CURSORON);
}
//turn off the cursor
void lcd_cursor_off(void) {
  lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON);
}
#endif

#if USE_LCD_CURSOR_BLINK == 1
//turn on cursor blinking
void lcd_blink_on(void) {
  lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON | LCDC_CURSORON | LCDC_BLINKON);
}
//turn off cursor blinking
void lcd - blink_off(void) {
  lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON);
}
#endif

//-----------------------------------------------------------------------

//LCD DISPLAY INITIALIZATION

void lcd_init(void) {
  data_dir_out();
  DDR(LCD_RSPORT) |= (1 << LCD_RS);
  DDR(LCD_EPORT) |= (1 << LCD_E);
#if USE_RW == 1
  DDR(LCD_RWPORT) |= (1 << LCD_RW);
#endif

  PORT(LCD_RSPORT) |= (1 << LCD_RS);
  PORT(LCD_EPORT) |= (1 << LCD_E);
#if USE_RW == 1
  PORT(LCD_RWPORT) |= (1 << LCD_RW);
#endif
  _delay_ms(15);
  PORT(LCD_RWPORT) &= ~(1 << LCD_E);
  PORT(LCD_RSPORT) &= ~(1 << LCD_RS);
#if USE_RW == 1
  PORT(LCD_EPORT) &= ~(1 << LCD_RW);
#endif
  SET_E;
  lcd_sendHalf(0x03);
  CLR_E;
  _delay_ms(4.1);
  SET_E;
  lcd_sendHalf(0x03);
  CLR_E;
  _delay_us(100);
  SET_E;
  lcd_sendHalf(0x02);
  CLR_E;
  _delay_us(100);
  lcd_write_cmd(LCDC_FUNC | LCDC_FUN4B | LCDC_FUN2L | LCDC_FUN5x7);
  lcd_write_cmd(LCDC_ONOFF | LCDC_CURSOROFF);
  lcd_write_cmd(LCDC_ONOFF | LCDC_DISPLAYON);
  lcd_write_cmd(LCDC_ENTRY | LCDC_ENTRYR);
  lcd_cls();
}
