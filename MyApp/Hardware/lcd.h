#ifndef __HW_LCD_H__
#define __HW_LCD_H__

#include "def.h"
#include "hw_def.h"

// PCF8574 기본 I2C 주소
#define LCD_DEV_ADDR 0x27 

bool LCD_init(I2C_HandleTypeDef *phI2C);
void LCD_writeStringXY(uint8_t row, uint8_t col, const char *str);
void LCD_clear(void);

#endif