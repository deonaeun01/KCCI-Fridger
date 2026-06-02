#include "ack.h"
#define OFFLINE_QUEUE_SIZE 50

static OfflineEvent_t offlineQueue[OFFLINE_QUEUE_SIZE];

static volatile uint16_t qHead = 0;
static volatile uint16_t qTail = 0;
static volatile bool g_server_connected = true;
static volatile uint32_t g_last_ack_tick = 0;

void ackInit(void)
{
    g_server_connected = true;
    g_last_ack_tick = HAL_GetTick();
}

void enqueueEvent(bool isOpen, RTC_TimeDef *time)
{
    uint16_t nextHead =
        (qHead + 1) % OFFLINE_QUEUE_SIZE;

    if(nextHead == qTail)
    {
        return;
    }

    offlineQueue[qHead].isOpen = isOpen;
    offlineQueue[qHead].time = *time;

    qHead = nextHead;
}

bool dequeueEvent(OfflineEvent_t *evt)
{
    if(qHead == qTail)
    {
        return false;
    }

    *evt = offlineQueue[qTail];

    qTail =
        (qTail + 1) % OFFLINE_QUEUE_SIZE;

    return true;
}

void ackReceive(void)
{
    g_last_ack_tick = HAL_GetTick();
    g_server_connected = true;
}

void ackCheckTimeout(void)
{
    if((HAL_GetTick() - g_last_ack_tick) > 5000)
    {
        g_server_connected = false;
    }
}

bool ackIsConnected(void)
{
    return g_server_connected;
}

void ackProcessOfflineQueue(void)
{
    static uint32_t lastSendTick = 0;

    if(!ackIsConnected())
    {
        return;
    }

    if(HAL_GetTick() - lastSendTick < 3000)
    {
        return;
    }

    lastSendTick = HAL_GetTick();

    OfflineEvent_t evt;

    if(dequeueEvent(&evt))
    {
        char msg[64];

        snprintf(msg,
                sizeof(msg),
                "%04d%02d%02d@%02d%02d%02d@%s",
                evt.time.year + 2000,
                evt.time.month,
                evt.time.date,
                evt.time.hour,
                evt.time.min,
                evt.time.sec,
                evt.isOpen ? "OPEN" : "CLOSE");

        btSendMessage(
            "KMS_RAS",
            "OFFLINE_EVENT",
            msg
        );
    }
}