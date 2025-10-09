#pragma once

#include "types.h"

/**
 * should contain functions of:
 *  - Create a data table with specific columns and formats. currently, time (HHmm), weight (unsigned integer, ranges 0-99,999, in grams)
 *  - Updates the table with a new data.
 *  - deletes the existing table.
 */


/** Here there's a problem that should be adressed:
 * It's prefered that the data is kept on the flash (4MB) but it's there with other stuff.
 * So after all code is done, should check how much space is left.
 * 
 * if it turns out problematic, maybe a string of number is prefered (3 bytes for hour, 4 bytes for weight)
 */
class DataTable {
    public:
        DataTable(uint16_t capacity);   // For now the size should be 14 hours * 60 readings an hour = 840 * struct Record size of space. the table is saved on the flash not RAM
        void createDataTable();
        void updateTable(Record record);
        Record *readTable();               // returns an array with the length of the number of records, each item is a record 
        void deleteTable();
    
    private:
        bool isFull();
};

