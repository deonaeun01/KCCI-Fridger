#include "bluetooth.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart6;

#define Q_SIZE 128      // 원형 큐 사이즈
#define MSG_MAX_LEN 64  // 한 패킷의 최대 길이
#define ARR_CNT 5

// Circular Queue 변수
static uint8_t rxQueue[Q_SIZE];
static volatile uint16_t qHead = 0; // enqueue 위치 (ISR 조작)
static volatile uint16_t qTail = 0; // dequeue 위치 (태스크 조작)

static uint8_t btRxChar;

// --- 큐 조작 내부 함수 ---
// 인터럽트에서 수신된 1바이트를 큐에 넣음
static void enqueue(uint8_t data)
{
    uint16_t nextHead = (qHead + 1) % Q_SIZE;
    
    // 큐가 꽉 차지 않았을 때만 저장 (Overflow 방지)
    if (nextHead != qTail) 
    {
        rxQueue[qHead] = data;
        qHead = nextHead;
    }
}

// 태스크에서 큐의 데이터를 1바이트씩 뺌
static bool dequeue(uint8_t *data)
{
    if (qHead == qTail) return false; // 큐가 비어있음

    *data = rxQueue[qTail];
    qTail = (qTail + 1) % Q_SIZE;
    return true;
}

// --- 하드웨어 제어 함수 ---
bool btInit(void)
{
    // 수신 인터럽트 최초 1회 가동
    if (HAL_UART_Receive_IT(&huart6, &btRxChar, 1) != HAL_OK) return false;
    return true;
}

// [인터럽트 루틴] 1바이트가 들어올 때마다 호출됨
void btRxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART6)
    {
        // 1. 받은 데이터를 즉시 큐에 enqueue
        enqueue(btRxChar);
        
        // 2. 다음 바이트를 받기 위해 인터럽트 재활성화
        HAL_UART_Receive_IT(&huart6, &btRxChar, 1);
    }
}

// [태스크 루틴] commTask에서 주기적으로 호출됨
void btProcess(void)
{
    // 태스크 내부에서 한 줄의 패킷을 조립하기 위한 정적 버퍼
    static char lineBuf[MSG_MAX_LEN];
    static uint8_t lineIdx = 0;
    uint8_t ch;

    // 큐에 데이터가 존재하는 동안 계속 dequeue 수행
    while (dequeue(&ch))
    {
        // 패킷의 끝을 알리는 문자 도착 시
        if (ch == '\n' || ch == '\r')
        {
            if (lineIdx == 0) continue; // 쓰레기(빈 줄) 무시

            lineBuf[lineIdx] = '\0';
            lineIdx = 0; // 다음 수신을 위해 인덱스 초기화

            // --- 문자열 파싱 시작 ---
            int i = 0;
            char *pArray[ARR_CNT] = {0};
            char *pToken = strtok(lineBuf, "[@]");
            
            while(pToken != NULL)
            {
                pArray[i] = pToken;
                if(++i >= ARR_CNT) break;
                pToken = strtok(NULL, "[@]");
            }

            if(i >= 3) 
            {
                // 서버 브로드캐스트 메시지 무시
                if(!strncmp(pArray[1], " New conn", 9) || !strncmp(pArray[1], " Already log", 12)) continue;

                if(!strcmp(pArray[1], "LED"))
                {
                    if(!strcmp(pArray[2], "ON"))
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
                    else if(!strcmp(pArray[2], "OFF"))
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                }
                else
                {
                    btSendMessage(pArray[0], pArray[1], pArray[2]);
                }
            }
        }
        else
        {
            // 아직 끝이 아니라면 버퍼에 문자 누적
            if (lineIdx < MSG_MAX_LEN - 1)
            {
                lineBuf[lineIdx++] = ch;
            }
        }
    }
}

void btSendMessage(const char* toId, const char* cmd, const char* val)
{
    char sendBuf[MSG_MAX_LEN];
    int len = snprintf(sendBuf, sizeof(sendBuf), "[%s]%s@%s\n", toId, cmd, val);
    
    if(len > 0)
    {
        HAL_UART_Transmit(&huart6, (uint8_t *)sendBuf, len, 50);
    }
}