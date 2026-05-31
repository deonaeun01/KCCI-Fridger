#include "lcd.h"
#include "cmsis_os.h" 

static uint8_t lcdData = 0;
static I2C_HandleTypeDef *phLcdI2C;

static void LCD_sendDataToInterface(void);
static void LCD_enableHigh(void);
static void LCD_enableLow(void);
static void LCD_sendData(uint8_t data);
static void LCD_sendByte(uint8_t data);
static void LCD_writeCmdData(uint8_t data);
static void LCD_writeCharData(uint8_t data);
static void LCD_cmdMode(void);
static void LCD_charMode(void);
static void LCD_writeMode(void);

bool LCD_init(I2C_HandleTypeDef *phI2C) {
    phLcdI2C = phI2C;

    osDelay(50); 
    LCD_cmdMode();
    LCD_writeMode();
    
    // 초기화 시퀀스는 무조건 osDelay로 넉넉히 기다려주어야 합니다.
    LCD_sendData(0x30);
    osDelay(5);
    LCD_sendData(0x30);
    osDelay(1);
    LCD_sendData(0x30);
    osDelay(1);
    LCD_sendData(0x20);
    osDelay(1);
    
    // 이 시점부터는 완벽한 4-bit 모드로 진입
    LCD_sendByte(0x28); 
    LCD_sendByte(0x08); 
    LCD_sendByte(0x01); 
    osDelay(2);         // Clear 명령은 LCD 내부적으로 오래 걸리므로 대기
    LCD_sendByte(0x06); 
    LCD_sendByte(0x0C); 
    
    // 백라이트 ON
    lcdData |= (1 << 3); 
    LCD_sendDataToInterface();
    
    return true;
}

// 핵심 수정 구간: 4비트 쪼개기 전송 시 태스크 양보 금지
static void LCD_sendData(uint8_t data) {
    LCD_enableHigh();
    
    // osDelay(2) 대신 마이크로초 단위의 짧은 대기 (OS 블로킹 없음)
    for(volatile int i = 0; i < 500; i++); 
    
    lcdData = (lcdData & 0x0F) | (data & 0xF0); // 데이터 세팅
    LCD_sendDataToInterface();
    
    LCD_enableLow(); // Falling Edge에서 LCD가 데이터를 확정적으로 낚아챔
    
    for(volatile int i = 0; i < 500; i++);
}

static void LCD_sendByte(uint8_t data) {
    // 1바이트를 반으로 쪼개서 연속으로 보냅니다.
    LCD_sendData(data);       
    LCD_sendData(data << 4);  
    
    // 한 글자(1바이트)를 온전히 전송한 후에만 CPU 제어권을 넘깁니다. (문자 깨짐 원천 차단)
    osDelay(1); 
}

static void LCD_writeCmdData(uint8_t data) {
    LCD_cmdMode();
    LCD_writeMode();
    LCD_sendByte(data);
}

static void LCD_writeCharData(uint8_t data) {
    LCD_charMode();
    LCD_writeMode();
    LCD_sendByte(data);
}

static void LCD_cmdMode(void)   { lcdData &= ~(1 << 0); LCD_sendDataToInterface(); }
static void LCD_charMode(void)  { lcdData |= (1 << 0);  LCD_sendDataToInterface(); }
static void LCD_writeMode(void) { lcdData &= ~(1 << 1); LCD_sendDataToInterface(); }
static void LCD_enableHigh(void){ lcdData |= (1 << 2);  LCD_sendDataToInterface(); }
static void LCD_enableLow(void) { lcdData &= ~(1 << 2); LCD_sendDataToInterface(); }

static void LCD_sendDataToInterface(void) {
    HAL_I2C_Master_Transmit(phLcdI2C, LCD_DEV_ADDR << 1, &lcdData, 1, 50);
}

void LCD_clear(void) {
    LCD_writeCmdData(0x01);
    osDelay(2);
}

void LCD_writeStringXY(uint8_t row, uint8_t col, const char *str) {
    col %= 16;
    row %= 2;
    uint8_t lcdRegAddr = ((0x40 * row) + col);
    LCD_writeCmdData(0x80 + lcdRegAddr); //

    for (int i = 0; str[i]; i++) {
        LCD_writeCharData(str[i]); //
    }
}