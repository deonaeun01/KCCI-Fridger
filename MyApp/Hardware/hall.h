#ifndef __HW_HALL_H__
#define __HW_HALL_H__

#include "def.h"
#include "hw_def.h"

void hallInit(void);
bool hallHasEvent(void);                  // 인터럽트 발생 여부 확인 및 플래그 초기화
bool hallIsOpen(void);                    // 현재 문의 물리적 상태(열림/닫힘) 확인
void hallExtiCallback(uint16_t GPIO_Pin); // HAL 인터럽트 콜백 연결용 함수

#endif