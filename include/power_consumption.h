# pragma once
#include "types.h"

/** This file deals with device power consumtion*/

class PwrConsumption {
    // see Table 4-1. Components and Power Domains in the ESP32-C3 Datasheet for more info.

public:
    /** Set Mode
     * 
     * Input:
     *  - PwrConsumptionMode mode.
     * 
     * Behaviour:
     *  1. set the mode to the requested mode
     *  2. set the flags to the default flags
     * 
     * Output:
     *  - None.
     * 
     * Display:
     *  - None.
     * 
     * Errors:
     *  - None
     * 
     * Notes:
     *  1. You may add more constants, functions, classes, etc. as needed.
    */
    static void setMode(PwrConsumptionMode mode);

    /** Get Mode
     * 
     * Input:
     *  - None
     * 
     * Behaviour:
     *  1. return the current mode 
     * 
     * Output:
     *  - PwrConsumptionMode mode.
     * 
     * Display:
     *  - None.
     * 
     * Errors:
     *  - None
     * 
     * Notes:
     *  1. You may add more constants, functions, classes, etc. as needed.
     */
    static PwrConsumptionMode getMode();

    /** Overwrite the current flags
     * 
     * Input:
     *  - None
     * 
     * Behaviour:
     *  1. according to the mode set, overwrite the current flags (see datasheet)
     * 
     * Output:
     *  - None.
     * 
     * Display:
     *  - None.
     * 
     * Errors:
     *  - None. if parts of call are illigal, silently ignore them
     * 
     * Notes:
     *  1. You may add more constants, functions, classes, etc. as needed.
     */
    static void setModeFlags(bool cpu, bool optDigPeriph, bool wlsDigCircuits);    // true = on, false = off

private:
    static PwrConsumptionMode mode;
    static bool cpu;
    static bool optDigPeriph;
    static bool wlsDigCircuits;
};

/** Power mode chage request
 * implement a mechanism that each task (sensing, networkings, scheduler, etc.) can "post" whatever mode it needs
 * and the system chooses the most demanding mode/config-flags.
 * 
 * Also note that each task should be responsible and for every given time its should ask for the least demanding mode.
 * For example, networkings asks for active mode. After all work is done networkings *should ask* for (e.g.) sleep mode
 * because, as for networkings' concern, it doesn't need more than that.
*/
