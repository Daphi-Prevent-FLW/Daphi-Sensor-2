# pragma once

/** 
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

void networkings();