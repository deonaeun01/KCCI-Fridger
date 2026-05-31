#include "ap.h"

// 태스크 간 자원 공유를 위한 volatile 전역 변수
volatile bool g_is_door_open = false; 
volatile uint32_t g_current_gas_mV = 0;
volatile uint8_t g_lcd_door_closed_timer = 0;

// 기본 태스크
void StartDefaultTask(void *argument)
{
    gasInit();
    btInit();
    hallInit();
    
    // [중요] 보드 전원 인가 후 LCD 칩셋이 부팅할 시간 0.5초 부여 (쓰레기값 방지)
    osDelay(500); 
    if(LCD_init(&hi2c1)) {
        LCD_writeStringXY(0, 0, "System Ready...");
    }
    osDelay(1000); // Ready 문구 1초 보여주기

    for(;;)
    {
        // sensorTask가 업데이트해둔 전역 변수를 보고 화면만 그립니다.
        if (g_is_door_open)
        {
            LCD_writeStringXY(0, 0, "Door Status:        ");
            LCD_writeStringXY(1, 0, "Door OPEN!      ");
        }
        else if (g_lcd_door_closed_timer > 0)
        {
            LCD_writeStringXY(0, 0, "Door Status:    ");
            LCD_writeStringXY(1, 0, "CLOSED!         ");
            g_lcd_door_closed_timer--; // 100ms마다 1씩 감소
        }
        else
        {
            char lcdBuf[17];
            snprintf(lcdBuf, sizeof(lcdBuf), "Gas: %4lu mV    ", g_current_gas_mV);
            LCD_writeStringXY(0, 0, "System Normal   ");
            LCD_writeStringXY(1, 0, lcdBuf);
        }
        
        // LCD는 100ms 주기로 부드럽게 갱신 (timer가 10이면 1초간 유지됨)
        osDelay(100); 
    }
}

// 센서들 통합 태스크
void sensorTask(void *argument)
{
    g_is_door_open = hallIsOpen(); 
    bool last_door_state = g_is_door_open; 
    
    uint32_t bounce_timer = 0; // 채터링 방지 타이머
    uint32_t sensor_timer = 0; // 1초 가스 전송 타이머

    for(;;)
    {
        // ① 홀센서 상태 읽기 및 소프트웨어 디바운싱 (중복 저장 완벽 차단)
        bool current_state = hallIsOpen();
        
        if (current_state != last_door_state) 
        {
            bounce_timer++;
            // 상태가 50ms (5 * 10ms) 동안 유지되어야만 진짜 변화로 인정
            if (bounce_timer > 5) 
            {
                last_door_state = current_state;
                g_is_door_open = current_state;

                if (g_is_door_open) 
                {
                    btSendMessage("KMS_RAS", "EVENT", "OPEN"); 
                }
                else 
                {
                    btSendMessage("KMS_RAS", "EVENT", "CLOSE"); 
                    g_lcd_door_closed_timer = 10; // LCD 태스크에 1초 알림 지시
                }
                
                // ★ DB 프리징 원천 차단: 이벤트를 보낸 직후 SENSOR 타이머를 0으로 리셋!
                // 이렇게 하면 EVENT 패킷 뒤에 SENSOR 패킷이 따라붙지 않고 무조건 1초의 침묵이 생깁니다.
                sensor_timer = 0; 
                bounce_timer = 0;
            }
        } 
        else 
        {
            bounce_timer = 0; // 흔들림이 멈추면 디바운스 카운터 초기화
        }

        // ② 주기적 센서 데이터 읽기 및 전송 (100 * 10ms = 1초)
        sensor_timer++;
        if (sensor_timer >= 100) 
        {
            sensor_timer = 0; // 1초 초기화
            
            gasDebugPrint(); 
            g_current_gas_mV = gasReadVoltage_mV(gasReadRaw());
            
            char valStr[64];
            snprintf(valStr, sizeof(valStr), "%lu@0.0@0.0", g_current_gas_mV);
            btSendMessage("KMS_RAS", "SENSOR", valStr);
        }

        osDelay(10); // 10ms 주기로 초고속 반응 대기
    }
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // 인터럽트 신호를 하드웨어 레이어로 전달
    hallExtiCallback(GPIO_Pin);
}

// HAL 라이브러리가 하드웨어 수신 완료 시 자동으로 호출해 주는 함수
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