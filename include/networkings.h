# pragma once

/** General information about this file
 * should handle all networkings ins and outs with all connected parties:
 *  - connect to wifi network
 *  - connect to main server
 *  - listen to messages from the main server.
 *      In order to save battery on listening, it should awake for listening in predetermined hours when it transmits data to server.
 *      So after any transmit, it should verify that no messages are sent from the server.
 *      It can also be awakened using Activate event triggered by a button-press
 *      (when already active, the event is used this way. see events.h::onActivate for more info).
 *  - send messages to main server
 *  - sync time with NTP server (pool.ntp.org)
 * 
 * Also note, that esp32 has fairly good api for this, so when implementing functions, there's no need to "reinvent the wheel".
 */

 /** The main networkings loop
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  0. First the scheduler should awaken the device for netwrkings (as we want the battery to last as much as possible)
 *  1. untill NetworkQueue is empty, dequeue each task and process it.
 *      a task is one of:
 *      - ConnectToWifi: establish conncection to wifi
 *      - ConnectToMainServer: establish connection to main server
 *      - SyncWithNTPServer: sync time with NTP server (pool.ntp.org). see events.h::onCalibrateClock
 *      - ListenToMainServer: listen for messages from the main server, enqueue events as needed
 *      - SendMsgToMainServer: send messages to the main server
 * 
 * Output:
 *  - void: No output
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 */
void networkings();