#include "ap.h"


// 기본 태스크
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  gasInit();
  btInit();
  if(LCD_init(&hi2c1)) {
        LCD_writeStringXY(0, 0, "System Ready...");
    }
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

// 센서들 통합 태스크
void sensorTask(void *argument)
{
  /* USER CODE BEGIN sensorTask */
  gasInit();
  /* Infinite loop */
  for(;;)
  {
    gasDebugPrint();
    uint16_t rawVal = gasReadRaw();
    uint32_t vol_mV = gasReadVoltage_mV(rawVal);
    char lcdBuf[17];
    // 뒤에 공백을 주어 이전 숫자의 잔상이 남지 않도록 덮어쓰기 처리
    snprintf(lcdBuf, sizeof(lcdBuf), "Gas: %4lu mV    ", vol_mV);
    LCD_writeStringXY(1, 0, lcdBuf);
    osDelay(500);
  }
  /* USER CODE END sensorTask */
}

// 블루투스 + 소켓 태스크
void commTask(void *argument)
{
  /* USER CODE BEGIN commTask */
  /* Infinite loop */
  for(;;)
  {
    btProcess();
    osDelay(10);
    
  }
  /* USER CODE END commTask */
}

// HAL 라이브러리가 하드웨어 수신 완료 시 자동으로 호출해 주는 약속된 함수
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 1. 만약 가스 센서 폴링/디버깅용 USART2에서 인터럽트를 쓴다면 여기에 작성
    // if(huart->Instance == USART2) 
    // {
    //     // USART2 처리 로직
    // }
    
    // 2. 블루투스 모듈이 연결된 USART6에서 인터럽트가 발생했다면?
    if(huart->Instance == USART6) 
    {
        // 하드웨어 레이어(bluetooth.c)에 만들어둔 큐 삽입 함수로 즉시 토스!
        btRxCpltCallback(huart); 
    }
}