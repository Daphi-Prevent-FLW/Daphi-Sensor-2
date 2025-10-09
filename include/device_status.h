#pragma once

#include <stdint.h>
#include <stdbool.h>

constexpr float MIN_BATTERY_POWER = 3.02; // in volts

// Atomic flag setters
void setIsActive(bool value); /* in `device_status.cpp`:
 * #include "freertos/atomic.h"
 * static volatile intptr_t isActiveFlag = 0;
 * void setIsActive(bool value) {atomic_store(&isActiveFlag, value ? 1 : 0);}
 * bool getIsActive(void) {return atomic_load(&isActiveFlag) != 0;}
 */
void setHasMainServerCommProblem(bool value);
void setHasDeviceId(bool value);

// Atomic flag getters
bool getIsActive(void);
bool getHasMainServerCommProblem(void);
bool getHasDeviceId(void);
