#pragma once

/**
 * This handles shceduling
 * Note that esp32, in order to track time, cannot be in deepsleep, but timers are active in deepsleep.
 * So in order to save battery, the logic should infer, when is the the next shceduled event, and put a timer to this time,
 * instead of tracking time of the day.
 * Also, Esp32 C3 has an internal better oscilator that drifts less (up to 5 sec a day comparing to 20 sec an hour)
 * (https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-reference/system/system_time.html)
 */

 /** Process responsible for scheduling
 * Input:
 *  - None
 * 
 * Behaviour:
 *  1. for each timed task (i.e., transmitting to server, synching with NTP server, etc.) a new timer should be set for the needed duration.
 *  2. when wakened, it should enqueue the corresponding event so the event listener can dequeue and run it.
 *  3. it should automatically reset a timer for the next awakaning time.
 * 
 * Output:
 *  - void: No output
 * 
 * Notes:
 *  1. The weight sensing is done in a different task, in order to make sure it's always performed.
 *  2. You may add more constants, functions, classes, etc. as needed.
 *  3. all times are stored in the non-voltile memory. (see config.h for more info.)
 */
void scheduler();