#ifndef __RTC_DS1307_H__
#define __RTC_DS1307_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

// =========================
// DS1307 I2C address
// =========================
#define DS1307_ADDR (0x68 << 1)

// =========================
// RTC data structure
// =========================
typedef struct {
    uint8_t year;   // 0~99
    uint8_t month;  // 1~12
    uint8_t date;   // 1~31
    uint8_t hour;   // 0~23
    uint8_t min;    // 0~59
    uint8_t sec;    // 0~59
} RTC_TimeDef;

// =========================
// API
// =========================
void rtcInit(void);
void rtcSetTime(RTC_TimeDef *t);
void rtcGetTime(RTC_TimeDef *t);

#endif