#pragma once
#include <cstdint>
#include "types.h"

constexpr uint16_t senseInterval = 60 * 1000;

constexpr uint8_t EVENTS_QUEUE_LENGTH = 10;

constexpr gpio HX711_DOUT       = 2;
constexpr gpio HX711_SCK        = 3;
constexpr gpio LED              = 4;
constexpr gpio BUTTON           = 5;    // make sure it's deepsleep wakeup enabaled
constexpr gpio BATTERY_POWER    = 0;    // change 0 to a valid number
