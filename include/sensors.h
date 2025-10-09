#pragma once

/** Process responsible for obtaining and logging load-cell readings
 * Input:
 *  - bool isActive: collect data only when device is active
 * 
 * Behaviour:
 *  1. read (digital) data from the input pins. see config.h constants about HX711 for more info.
 *      - validate input is not corrupted
 *  2. log to the sensor-table with HHmm (24 hours format, no ":") timestamp. see data.h for more info.
 *  3. sleep until the next interval (config.h senseInterval)
 * 
 * Output:
 *  - void: No output
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 *  2. This process isn't on the main process (aka, void loop) to make sure no gaps, that may result from other processes, will appear in the data.
 */
void getLoadCellData(bool isActive);

/** Invoked by other functions, responsible for obtaining battery power (in Volts)
 * Input:
 *  - bool isActive: collect data only when device is active
 * 
 * Behaviour:
 *  1. read (analog) data from the input pin. see config.h constants about battery power for more info.
 *      - validate input is not corrupted
 * 
 * Output:
 *  - float: power in volts
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 */
float getBatteryPower(bool isActive);
