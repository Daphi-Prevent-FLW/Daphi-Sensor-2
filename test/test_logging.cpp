// unit test file

/** Implement and test:
 * Given: a log file instace with a size N - constructor
 * When: we create a new log file
 * Then: the new log file has deviceID and the current date formatted: dd/mm/YYYY
 */

/** Implement and test:
 * Given: a log file
 * When: we add a log row 
 * Then: we get the previous log file and below it, the added row.
 */

/** Implement and test:
 * Given: a log file
 * When: we add a date 
 * Then: we get the previous log file and below it the added date
 */

/** Implement and test:
 * Given: a log file
 * When: we add dates more then the size of the file, and read the file
 * Then: we can no longer update the file and the last row has the meaning of "logfile is full, yet more info is tried to be logged"
 */

/** Implement and test:
 * Given: a log file
 * When: we add rows more then the size of the file, and read the file
 * Then: we can no longer update the file and the last row has the meaning of "logfile is full, yet more info is tried to be logged"
 */

/** Implement and test:
 * Given: a non empty log file (with some rows and dates)
 * When: we delete the log file and then read it
 * Then: we get a clean log file (i.e., deviceID and current date)
 */