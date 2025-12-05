//Piotr Makulec, 2025
//Written for and tested on Arduino (ATmega328p).

#ifndef LCD_H_
#define LCD_H_

#define LCD_Y 2       //amount of rows in the diplay
#define LCD_X 16      //amount of columns in the display

/*  RW pin:
 *  0 - connected to the ground
 *  1 - connected to the uC
 */
#define USE_RW 1      

//port config
#define LCD_D7PORT D
#define LCD_D7 7
#define LCD_D6PORT D
#define LCD_D6 6
#define LCD_D5PORT D
#define LCD_D5 5
#define LCD_D4PORT D
#define LCD_D4 4
#define LCD_EPORT D
#define LCD_E 3 
#define LCD_RWPORT D
#define LCD_RW 2 
#define LCD_RSPORT D
#define LCD_RS 1

//---------------------------------------------------------------------------------
//COMPILATION SETTINGS

#define USE_LCD_LOCATE 1        //sets cursor on chosen position Y (0-3), X(0-n)

#define USE_LCD_CHAR 1          //sends char as an argument to the function
#define USE_LCD_STR_P 1         //sends string from FLASH
#define USE_LCD_STR_E 1         //sends string from EEPROM

#define USE_LCD_INT 1           //sends decimal number to LCD
#define USE_LCD_HEX 1           //sends hexagonal number to LCD

#define USE_LCD_DEFCHAR 1       //sends defined character from RAM
#define USE_LCD_DEFCHAR_P 1     //sends defined character from FLASH
#define USE_LCD_DEFCHAR_E 1     //sends defined character from EEPROM

#define USE_LCD_CURSOR_ON 0     //cursor on/off
#define USE_LCD_CURSOR_BLINK 0  //cursor blinking on/off
#define USE_LCD_CURSOR_HOME 0   //sets cursor at the start

//---------------------------------------------------------------------

/*  DDRAM addresses' definitions
 *  different for 2 and 4-row diplays)
 */
#if ((LCD_Y == 4) && (LCD_X == 20))
#define LCD_LINE1 0x00  //address of the 1 char in 1 row
#define LCD_LINE2 0x28  //etc.
#define LCD_LINE3 0x14
#define LCD_LINE4 0x54
#else
#define LCD_LINE1 0x00
#define LCD_LINE2 0x40
#define LCD_LINE3 0x10
#define LCD_LINE4 0x50
#endif

//Port access macros:
//PORT
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
//PIN
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
//DDR
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)

//control commands
#define LCDC_CLS                    0x01
#define LCDC_HOME                   0x02
#define LCDC_ENTRY                  0x04
  #define LCDC_ENTRYR                       0x02
  #define LDCD_ENTRYL                       0
  #define LDCD_MOVE                         0x01
#define LCDC_ONOFF                  0x08
  #define LCDC_DISPLAYON            0x04
  #define LCDC_CURSORON             0x02
  #define LCDC_CURSOROFF            0
  #define LCDC_BLINKON              0x01
#define LCDC_SHIFT                  0x10
  #define LCDC_SHIFTDISP            0x08
  #define LCDC_SHIFTR                       0x40
  #define LCDC_SHIFTL                       0
#define LCDC_FUNC                   0x20
  #define LCDC_FUN8B                        0x10
  #define LCDC_FUN4B                        0
  #define LCDC_FUN2L                        0x08
  #define LCDC_FUN1L                        0
  #define LCDC_FUN5x10              0x04
  #define LCDC_FUN5x7               0
#define LCDC_SET_CGRAM      0x40
#define LCDC_SET_DDRAM      0x80

// function declarations
void lcd_init(void);                    // turned on for compilation
void lcd_cls(void);                     // turned on for compilation
void lcd_str(char * str);               // turned on for compilation

void lcd_locate(uint8_t y, uint8_t x);                    // turned on by default for compilation
void lcd_str_P(const char * str);                         // turned off by default for compilation in lcd_display.c
void lcd_str_E(char * str);                               // turned off by default for compilation in lcd_display.c
void lcd_int(int val);                                    // turned off by default for compilation in lcd_display.c
void lcd_hex(uint32_t val);                               // turned off by default for compilation in lcd_display.c
void lcd_defchar(uint8_t nr, uint8_t *def_znak);          // turned off by default for compilation in lcd_display.c
void lcd_defchar_P(uint8_t nr, const uint8_t *def_znak);  // turned off by default for compilation in lcd_display.c
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak);        // turned off by default for compilation in lcd_display.c

void lcd_home(void);                    // turned off by default for compilation in lcd_display.c
void lcd_cursor_on(void);               // turned off by default for compilation in lcd_display.c
void lcd_cursor_off(void);              // turned off by default for compilation in lcd_display.c
void lcd_blink_on(void);                // turned off by default for compilation in lcd_display.c
void lcd_blink_off(void);               // turned off by default for compilation in lcd_display.c

#endif
