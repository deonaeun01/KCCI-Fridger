#include "hall.h"

// 인터럽트와 태스크 간 동기화를 위한 전역 변수
static volatile bool hallEventFlag = false;

void hallInit(void)
{
    hallEventFlag = false;
}

bool hallHasEvent(void)
{
    if (hallEventFlag)
    {
        hallEventFlag = false;
        return true;
    }
    return false;
}

// 현재 문의 상태 리턴 (센서 모듈 특성에 따라 로직 변경 가능)
bool hallIsOpen(void)
{
    // 예시: 자석이 멀어짐(열림) = HIGH(GPIO_PIN_SET) 이라 가정
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        return true; // 문 열림
    }
    return false;    // 문 닫힘
}

// 물리적 하드웨어 인터럽트 발생 시 호출되는 콜백
void hallExtiCallback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_0) 
    {
        hallEventFlag = true;
    }
}