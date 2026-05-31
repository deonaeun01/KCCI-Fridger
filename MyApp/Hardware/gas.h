#ifndef __MAP_HW__GAS_H__
#define __MAP_HW__GAS_H__

#include "def.h"
#include "hw_def.h"

bool gasInit(void);
uint16_t gasReadRaw(void);
uint32_t gasReadVoltage_mV(uint16_t raw);
void gasDebugPrint(void);

#endif