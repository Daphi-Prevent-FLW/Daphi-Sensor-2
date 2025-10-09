# pragma once

#include "types.h"

/**
 * should contain functions of:
 *  - Create a log file. It should contain deviceID and date.
 *      If log file is spanned across several days, e.g., from Jan 1st 17:00 till Jan 2nd 07:00, date should be updated as well
 *      Time and dates should always be UTC+0 without daylight saving time. Server will adjust times and dates accordingly.
 *  - Updates the log file with a new data.
 *  - Deletes the existing table.
 * 
 * Example log file
 * deviceID: <deviceID>
 * 
 * date: <date>
 * <here all data should be listed, see onCheckDeviceStatus for more info>
 * 
 * date: <date>
 * <here all data should be listed, see onCheckDeviceStatus for more info>
 */

/** Here there's a problem that should be adressed:
 * It's prefered that the data is kept on the flash (4MB) but it's there with other stuff.
 * So after all code is done, should check how much space is left.
 */

class LogFile {
    public:
        LogFile(uint16_t size);     // size of the file
        void createLogFile();
        void addLogRow(const char *msg);
        void addDate(dateType date);
        const char *readLogFile();
        void deleteLogFile();

    private:
        bool isFull();  // should alert right before it's full and log message: "logfile is full, yet more info is tried to be logged"
};

/** what should be logged:
 * any event with HHmm timestamp, any error - critical or not - with HHmm timestamp
 * including when transmitting data \ log file to server (log the transmission before transmitting - allows follow up if communication fails) 
 * 
 * to shorten the file, codes may be used instead of strings.
 * list all the possible log info in an enum class but be sure you know what each value is mapping to.
 * This will be later decoded in the server.
 */