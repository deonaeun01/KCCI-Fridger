#ifndef __APP_ACK_H__
#define __APP_ACK_H__
#include "rtc.h"
#include "def.h"
#include "hw_def.h"

typedef struct
{
    bool isOpen;
    RTC_TimeDef time;
} OfflineEvent_t;

void enqueueEvent(bool isOpen, RTC_TimeDef *time);
bool dequeueEvent(OfflineEvent_t *evt);

void ackInit(void);
void ackReceive(void);
void ackCheckTimeout(void);
bool ackIsConnected(void);
void ackProcessOfflineQueue(void);
#endif