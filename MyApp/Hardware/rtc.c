#include "rtc.h"

// 외부 I2C 핸들
extern I2C_HandleTypeDef hi2c2;

static uint8_t dec_to_bcd(uint8_t val)
{
    return ((val / 10) << 4) | (val % 10);
}

static uint8_t bcd_to_dec(uint8_t val)
{
    return ((val >> 4) * 10) + (val & 0x0F);
}

#define REG_SEC   0x00
#define REG_MIN   0x01
#define REG_HOUR  0x02
#define REG_DAY   0x03
#define REG_DATE  0x04
#define REG_MON   0x05
#define REG_YEAR  0x06
#define REG_CTRL  0x07

// =========================
// low-level I2C helpers
// =========================
static HAL_StatusTypeDef write_reg(uint8_t reg, uint8_t data)
{
    return HAL_I2C_Mem_Write(&hi2c2,
                             DS1307_ADDR,
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             &data,
                             1,
                             100);
}

static HAL_StatusTypeDef read_reg(uint8_t reg, uint8_t *data)
{
    return HAL_I2C_Mem_Read(&hi2c2,
                            DS1307_ADDR,
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            data,
                            1,
                            100);
}

void rtcInit(void)
{
    uint8_t sec;

    // CH bit clear -> oscillator start
    read_reg(REG_SEC, &sec);
    sec &= 0x7F;
    write_reg(REG_SEC, sec);

    // optional: control register clear (SQW off)
    write_reg(REG_CTRL, 0x00);
}

void rtcSetTime(RTC_TimeDef *t)
{
    uint8_t buf[7];

    buf[0] = dec_to_bcd(t->sec) & 0x7F; // CH bit 0
    buf[1] = dec_to_bcd(t->min);
    buf[2] = dec_to_bcd(t->hour);
    buf[3] = 0x01; // day dummy (not used often)
    buf[4] = dec_to_bcd(t->date);
    buf[5] = dec_to_bcd(t->month);
    buf[6] = dec_to_bcd(t->year);

    HAL_I2C_Mem_Write(&hi2c2,
                      DS1307_ADDR,
                      REG_SEC,
                      I2C_MEMADD_SIZE_8BIT,
                      buf,
                      7,
                      100);
}

void rtcGetTime(RTC_TimeDef *t)
{
    uint8_t buf[7];

    HAL_I2C_Mem_Read(&hi2c2,
                     DS1307_ADDR,
                     REG_SEC,
                     I2C_MEMADD_SIZE_8BIT,
                     buf,
                     7,
                     100);

    t->sec   = bcd_to_dec(buf[0] & 0x7F);
    t->min   = bcd_to_dec(buf[1]);
    t->hour  = bcd_to_dec(buf[2]);
    t->date  = bcd_to_dec(buf[4]);
    t->month = bcd_to_dec(buf[5]);
    t->year  = bcd_to_dec(buf[6]);
}