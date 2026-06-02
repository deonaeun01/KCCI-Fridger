#ifndef __HW_BLUETOOTH_H__
#define __HW_BLUETOOTH_H__

#include "ack.h"
#include "def.h"
#include "hw_def.h"

bool btInit(void);
void btProcess(void);

// 가스 센서 값 등 외부로 데이터를 보낼 때 사용할 API
void btSendMessage(const char* toId, const char* cmd, const char* val);

// UART RX 인터럽트 콜백 연결용 함수
void btRxCpltCallback(UART_HandleTypeDef *huart);

#endif