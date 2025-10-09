#pragma once

/**
 * This handles shceduling
 * Note that esp32, in order to track time, cannot be in deepsleep, but timers are active in deepsleep.
 * So in order to save battery, the logic should infer, when is the the next shceduled event, and put a timer to this time,
 * instead of tracking time of the day.
 * Also, Esp32 C3 has an internal better oscilator that drifts less (up to 5 sec a day comparing to 20 sec an hour)
 * (https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-reference/system/system_time.html)
 */

void scheduler();