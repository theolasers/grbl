#include "lcd7920.h"

//Font Generated by MikroElektronika GLCD Font Creator 1.2.0.0
//MikroElektronika 2011 
//http://www.mikroe.com 

//GLCD FontName : glcd10x10
//GLCD FontSize : 10 x 10

// Note that we replace the fullstop character by decimal point so that the Arduino print functions display a decimla point when printing numbers.
static const prog_uint8_t glcd10x10[] PROGMEM = {
        0x04, 0x06, 0x00, 0x09, 0x00, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for degree symbol 
        0x09, 0x38, 0x00, 0x45, 0x00, 0x83, 0x00, 0x07, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x82, 0x01, 0x44, 0x01, 0x38, 0x00, 0x00, 0x00,  // Code for rotating arrows 1 
        0x09, 0xE0, 0x00, 0x64, 0x00, 0xA2, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x8A, 0x00, 0x4C, 0x00, 0x0E, 0x00, 0x00, 0x00,  // Code for rotating arrows 2
        0x0A, 0x10, 0x00, 0x10, 0x00, 0xFF, 0x01, 0xFE, 0x00, 0x7C, 0x00, 0x38, 0x00, 0x10, 0x00, 0xFF, 0x01, 0x10, 0x00, 0x10, 0x00,  // Code for diode 
        0x08, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0xFF, 0x01, 0x28, 0x00, 0x44, 0x01, 0x82, 0x01, 0xC1, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for transistor 
        0x07, 0x30, 0x00, 0x60, 0x00, 0xC0, 0x00, 0x60, 0x00, 0x18, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for tick 
        0x02, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for full stop 
        0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x4C, 0x00, 0x4E, 0x00, 0x3F, 0x00, 0x0E, 0x00, 0x0C, 0x00, 0x08, 0x00,  // Code for return
        0x0A, 0x28, 0x00, 0x38, 0x00, 0x38, 0x00, 0x38, 0x00, 0x38, 0x00, 0xFF, 0x01, 0xFE, 0x00, 0x7C, 0x00, 0x38, 0x00, 0x10, 0x00,  // Code for enter
        0x03, 0xFF, 0x00, 0x81, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 
        0x03, 0xFF, 0x00, 0x81, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 
        0x03, 0xFF, 0x00, 0x81, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 
        0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char  
        0x01, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char !
        0x03, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char "
        0x05, 0x24, 0x00, 0xFF, 0x00, 0x24, 0x00, 0xFF, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char #
        0x05, 0x46, 0x00, 0x89, 0x00, 0xFF, 0x01, 0x89, 0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char $
        0x09, 0x06, 0x00, 0x09, 0x00, 0x89, 0x00, 0x66, 0x00, 0x18, 0x00, 0x66, 0x00, 0x91, 0x00, 0x90, 0x00, 0x60, 0x00, 0x00, 0x00,  // Code for char %
        0x06, 0x60, 0x00, 0x96, 0x00, 0x89, 0x00, 0x99, 0x00, 0x66, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char &
        0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char '
        0x03, 0xFC, 0x00, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char (
        0x03, 0x01, 0x02, 0x02, 0x01, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char )
        0x05, 0x15, 0x00, 0x0E, 0x00, 0x1F, 0x00, 0x0E, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char *
        0x05, 0x10, 0x00, 0x10, 0x00, 0x7C, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char +
        0x02, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ,
        0x03, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char -
        0x03, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char . (extra space left and right)
        0x03, 0xC0, 0x00, 0x3C, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char /
        0x05, 0x7E, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 0
        0x03, 0x04, 0x00, 0x02, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 1
        0x05, 0x82, 0x00, 0xC1, 0x00, 0xA1, 0x00, 0x91, 0x00, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 2
        0x05, 0x42, 0x00, 0x81, 0x00, 0x89, 0x00, 0x89, 0x00, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 3
        0x06, 0x30, 0x00, 0x28, 0x00, 0x24, 0x00, 0x22, 0x00, 0xFF, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 4
        0x05, 0x4F, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 5
        0x05, 0x7E, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 6
        0x05, 0x01, 0x00, 0xE1, 0x00, 0x19, 0x00, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 7
        0x05, 0x76, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 8
        0x05, 0x4E, 0x00, 0x91, 0x00, 0x91, 0x00, 0x91, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char 9
        0x01, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char :
        0x01, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ;
        0x05, 0x10, 0x00, 0x10, 0x00, 0x28, 0x00, 0x28, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char <
        0x05, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char =
        0x05, 0x44, 0x00, 0x28, 0x00, 0x28, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char >
        0x05, 0x02, 0x00, 0x01, 0x00, 0xB1, 0x00, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ?
        0x0A, 0xF8, 0x00, 0x04, 0x01, 0x72, 0x02, 0x89, 0x02, 0x85, 0x02, 0x45, 0x02, 0xF9, 0x02, 0x8D, 0x02, 0x42, 0x02, 0x3C, 0x01,  // Code for char @
        0x07, 0xC0, 0x00, 0x30, 0x00, 0x2C, 0x00, 0x23, 0x00, 0x2C, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char A
        0x06, 0xFF, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char B
        0x06, 0x3C, 0x00, 0x42, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char C
        0x06, 0xFF, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x42, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char D
        0x05, 0xFF, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char E
        0x05, 0xFF, 0x00, 0x09, 0x00, 0x09, 0x00, 0x09, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char F
        0x07, 0x3C, 0x00, 0x42, 0x00, 0x81, 0x00, 0x81, 0x00, 0x91, 0x00, 0x52, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char G
        0x06, 0xFF, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char H
        0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char I
        0x04, 0x60, 0x00, 0x80, 0x00, 0x80, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char J
        0x06, 0xFF, 0x00, 0x10, 0x00, 0x08, 0x00, 0x1C, 0x00, 0x62, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char K
        0x05, 0xFF, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char L
        0x07, 0xFF, 0x00, 0x06, 0x00, 0x18, 0x00, 0x60, 0x00, 0x18, 0x00, 0x06, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char M
        0x06, 0xFF, 0x00, 0x02, 0x00, 0x0C, 0x00, 0x30, 0x00, 0x40, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char N
        0x07, 0x3C, 0x00, 0x42, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x42, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char O
        0x05, 0xFF, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char P
        0x07, 0x3C, 0x00, 0x42, 0x00, 0x81, 0x00, 0xA1, 0x00, 0xA1, 0x00, 0x42, 0x00, 0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char Q
        0x06, 0xFF, 0x00, 0x09, 0x00, 0x09, 0x00, 0x19, 0x00, 0x69, 0x00, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char R
        0x06, 0x46, 0x00, 0x89, 0x00, 0x89, 0x00, 0x91, 0x00, 0x91, 0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char S
        0x05, 0x01, 0x00, 0x01, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char T
        0x06, 0x7F, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char U
        0x07, 0x03, 0x00, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x30, 0x00, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char V
        0x0A, 0x03, 0x00, 0x3C, 0x00, 0xC0, 0x00, 0x30, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x3C, 0x00, 0x03, 0x00,  // Code for char W
        0x06, 0x81, 0x00, 0x66, 0x00, 0x18, 0x00, 0x18, 0x00, 0x66, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char X
        0x07, 0x01, 0x00, 0x06, 0x00, 0x08, 0x00, 0xF0, 0x00, 0x08, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char Y
        0x06, 0x81, 0x00, 0xC1, 0x00, 0xB1, 0x00, 0x8D, 0x00, 0x83, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char Z
        0x02, 0xFF, 0x03, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char [
        0x03, 0x03, 0x00, 0x3C, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char BackSlash
        0x02, 0x01, 0x02, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ]
        0x05, 0x08, 0x00, 0x06, 0x00, 0x01, 0x00, 0x06, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ^
        0x06, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char _
        0x02, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char `
        0x05, 0x68, 0x00, 0x94, 0x00, 0x94, 0x00, 0x54, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char a
        0x05, 0xFF, 0x00, 0x48, 0x00, 0x84, 0x00, 0x84, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char b
        0x05, 0x78, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char c
        0x05, 0x78, 0x00, 0x84, 0x00, 0x84, 0x00, 0x48, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char d
        0x05, 0x78, 0x00, 0x94, 0x00, 0x94, 0x00, 0x94, 0x00, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char e
        0x03, 0x04, 0x00, 0xFE, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char f
        0x05, 0x78, 0x02, 0x84, 0x02, 0x84, 0x02, 0x48, 0x02, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char g
        0x05, 0xFF, 0x00, 0x08, 0x00, 0x04, 0x00, 0x04, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char h
        0x01, 0xFD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char i
        0x02, 0x00, 0x02, 0xFD, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char j
        0x04, 0xFF, 0x00, 0x10, 0x00, 0x68, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char k
        0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char l
        0x07, 0xFC, 0x00, 0x08, 0x00, 0x04, 0x00, 0xFC, 0x00, 0x04, 0x00, 0x04, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char m
        0x05, 0xFC, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char n
        0x05, 0x78, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char o
        0x05, 0xFC, 0x03, 0x48, 0x00, 0x84, 0x00, 0x84, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char p
        0x05, 0x78, 0x00, 0x84, 0x00, 0x84, 0x00, 0x48, 0x00, 0xFC, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char q
        0x03, 0xFC, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char r
        0x05, 0x48, 0x00, 0x94, 0x00, 0x94, 0x00, 0xA4, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char s
        0x02, 0xFF, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char t
        0x05, 0x7C, 0x00, 0x80, 0x00, 0x80, 0x00, 0x40, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char u
        0x05, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x30, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char v
        0x09, 0x0C, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x30, 0x00, 0x08, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x30, 0x00, 0x0C, 0x00, 0x00, 0x00,  // Code for char w
        0x05, 0x84, 0x00, 0x48, 0x00, 0x30, 0x00, 0x48, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char x
        0x05, 0x0C, 0x00, 0x30, 0x02, 0xC0, 0x01, 0x30, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char y
        0x05, 0x84, 0x00, 0xC4, 0x00, 0xB4, 0x00, 0x8C, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char z
        0x03, 0x10, 0x00, 0xEE, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char {
        0x01, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char |
        0x04, 0x00, 0x00, 0x01, 0x02, 0xEE, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char }
        0x05, 0x10, 0x00, 0x08, 0x00, 0x18, 0x00, 0x10, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ~
        0x03, 0xFF, 0x00, 0x81, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   // Code for char 
        };

extern const PROGMEM LcdFont font10x10 =
{
  glcd10x10,   // font data
  0x10,        // first character code
  0x7F,        // last character code
  10,          // row height in pixels
  10           // character width in pixels
};


// End

