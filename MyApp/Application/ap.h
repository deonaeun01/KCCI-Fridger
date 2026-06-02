#ifndef __MY_AP__AP_H__
#define __MY_AP__AP_H__

#include "def.h"
#include "hw_def.h"
#include "ack.h"

void StartDefaultTask(void *argument);
void sensorTask(void *argument);
void commTask(void *argument);
#endif