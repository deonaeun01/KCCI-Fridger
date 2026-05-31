#include "gas.h"

extern ADC_HandleTypeDef hadc1; 
extern UART_HandleTypeDef huart2;

bool gasInit(void)
{
    char *bootMsg = "\r\n[System Start] Gas Sensor Polling Mode...\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)bootMsg, 43, 100);
    
    return true;
}

uint16_t gasReadRaw(void)
{
    uint16_t adcValue = 0;
    
    HAL_ADC_Start(&hadc1);
    
    // 10ms 동안 변환을 기다림
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        adcValue = HAL_ADC_GetValue(&hadc1);
    }
    
    HAL_ADC_Stop(&hadc1);
    
    return adcValue;
}

uint32_t gasReadVoltage_mV(uint16_t raw)
{
    return (uint32_t)((raw * 3300) / 4095);
}

void gasDebugPrint(void)
{
    uint16_t rawVal = gasReadRaw();
    uint32_t vol_mV = gasReadVoltage_mV(rawVal);
    
    char txBuffer[64];
    
    int len = snprintf(txBuffer, sizeof(txBuffer), "[MQ-135] Raw: %4u | Vol: %4lu mV\r\n", rawVal, vol_mV);
    
    if (len > 0) {
        HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, len, 50);
    }
}