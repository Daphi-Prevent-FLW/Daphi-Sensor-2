#pragma once

// Please note that there're many files, each handles differnet matter.
// Every aspect that requires one the following parts, need to be implemented using the given interface
// if no interface is given (comments-only file) please notify us.
#include "config.h"         // some constants and configurations
#include "device_status.h"  // flags about device status and their function
#include "display.h"        // handles the display task
#include "events.h"         // handles events
#include "sensors.h"        // handles data from the event
#include "types.h"          // project-specific types and structs
#include "queue.h"          // just queue class for now
#include "data.h"           // functions to handle the sensor data table
#include "scheduler.h"      // functions to handle scheduling tasks, like sending data to server
#include "networkings.h"    // functions to handle networking tasks

/** On the main process - an event listener
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. When an event happens, enqueue it in the events queue
 * 
 * Output:
 *  - void: No output
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 *  2. An event may happen from button press (i.e., setup: 3sec press + is connected to PC, activate: double press, check-device-status: short press),
 *      from the main server (i.e., change-transmission-time, calibrate, deactivate, sendLogFiles, sendData, check-device-status),
 *      by scheduler (i.e., sendData, sendLogFiles) or by other event.
 *  3. If this process should be parallel to the main loop, you're encourgaed to share your thoughts.
 */
void listenToEvents();

/** The setup logic
 * This function must be invoked only if the device is connected to a computer via the USB (serial) port.
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. If no wifi networks are stored, the device asks for the number of wifi networks to be prompted.
 *      1.1. The device asks repeatedly for the SSID and password of the network - the user answers
 *          - All stored in the EEPROM; passwords should be stored safely
 *  2. If there are wifi networks, the device asks if user wants to add or delete a network (changing password is by delete and re-write)
 *      2.1. if delete, then the device prompts all known networks (SSID only, no passwords) - the user enters the SSIDs,
 *              seperated by spaces, of the networks to be deleted, or returns (i.e., presses enter) if no network's to be deleted.
 *      2.2. if add, then do step 1. 
 *  3. If no main server IP is stored, the device asks for the IP adress of the main server (which stores the data) - the user answers
 *      - it's stored in the EEPROM
 *  4. If there's a main server IP, it asks for changing of the IP - if user returns yes, do step 3. 
 *  5. If no NTP server IP, the device asks for IP adress of an NTP server - the user answers
 *      - it's stored in the EEPROM
 *  6. If there's an NTP server IP, it asks for changing of the IP - if user returns yes, do step 5.
 *  7. The device asks the for the weight of the weighihg plate (ontop which the garbage is placed).
 *      - if the number is not between [PLATE_TOL_MIN, PLATE_TOL_MAX], prompt error to user (sometihng like 'invalid weight, please re-enter its weight')
 *      - the number is stored temporarly on the EEPROM until it's sent to server.
 *  8. The device initiates wifi communication with the servers.
 *  9. The device syncs time and date with the NTP server.
 *  10. ONLT IF the device LACKS a deviceID, it asks the main server to give it a deviceID
 *      - deviceID is assigned by the main server to make sure it's unique and to log some other details about the device.
 *      - The deviceID is stored in the OTP-memory (in debug mode, on the standard EEPROM)
 *      - if it has a deviceID, it prompts it's deviceID.
 *      - use device_status.h::set\getDeviceID to determine and change it accordingly.
 *  11. the device sends its plate weight (from step 7.) to the server.
 *  
 * Output:
 *  - None.
 * 
 * Display:
 *  - All communication with the user is via the serial prompt.
 * 
 * Errors:
 *  - All errors (malconnections, corrupted input, user typos, unable to save into memory, etc.) must be prompted to the user on the screen.
 *  - If already activated, device should be de activated for setup, and then (re)activeted.
 *      So events queue should be (first to dequeued: ) ... deactivte, setup, activate, ... (last).
 *      This is to make sure no garbage or error-prone messages are sent to the server
 *  
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 *  2. Display is always on computer-mode, even when defined otherwise.
*/
void onSetup();

/** The device activation logic
 * Activation differs from setup in that, during setup, some information that is device-dependent,
 * and sometimes, deployment-dependent is introduced to the device, while activation tells the device that it's deployed
 * and should measure and transmit its measurements to the server. It also tells the server to expect transmissions from this device.
 * A device may be setted up once, but activated and deactivated multiple times.
 * 
 * if the device is already active, the event will be used to awake the device to listen to messages from the server.
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. The device sends to the main server a text message containing its deviceID, a datetime stamp and "activated" string.
 *  2. Awaits confirmation from the main server.
 *  3. if no log file - create a new log file (aka constructor)
 *  4. if no data table - create a new data table (aka constructor)
 *  5. if not already active - two events with priority 1 (highest) are enqueued, one with EventType::ChangeTxTimes,
 *      and the other with EventType::Calibrate.
 *      - if already active, it's just used to awaken the device to listen to messages from the server.
 *  6. finally, use device_status.h::setIsActive to set the device active
 *  
 * Output:
 *  - None.
 * 
 * Display:
 *  - Display is LEDOnly, ComputerOnly or Both as defined before compilation.
 *  - If it's LEDOnly / Both, a LEDPattern of activation should be displayed.
 *  - If it's ComputerOnly / Both, the following msgs should be presented to the user:
 *      - Activation begins
 *      - Sending to the main server <IP of the main server>: <deviceID> <datetime stamp> activated
 *      - (if confirmation's recieved) Activation confirmation's received on <datetime stamp of recieveing>
 *  - on deployment, device is always LEDOnly 
 * 
 * Errors:
 *  - activation should take place after a successful setup. If device isn't successfully setted up, it should stop activation and envoke setup
 *  - All errors (malconnections, etc.) must be prompted to the user.
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
*/
void onActivate();

/** The device deactivation logic
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. The device sends to the main server a text message containing its deviceID, a datetime stamp and "deactivated" string.
 *  2. Awaits confirmation from the main server.
 *  3. it then sends all the log file and all its data table and deletes the existing file and table.
 *  
 * Output:
 *  - None.
 * 
 * Display:
 *  - Display is LEDOnly, ComputerOnly or Both as defined before compilation.
 *  - If it's LEDOnly / Both, a LEDPattern of activation should be displayed.
 *  - If it's ComputerOnly / Both, the following msgs should be presented to the user:
 *      - Deactivation begins
 *      - Sending to the main server <IP of the main server>: <deviceID> <datetime stamp> deactivated
 *      - (if confirmation's recieved) Deactivation confirmation's received on <datetime stamp of recieveing>
 *  - on deployment, device is always LEDOnly 
 * 
 * Errors:
 *  - Deactivation should take place after a successful setup. If device isn't successfully setted up, it should stop deactivation and envoke setup
 *  - All errors (malconnections, etc.) must be prompted to the user.
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
*/
void onDeactivate();

/** Checking device status logic
 * Device status has two levels:
 *  1. Critical - enough battery, goodenough communication, no shortcuts, no unconnected peripherials
 *  2. All others
 * Only problems with level 1 are to be displayed on LED, all others (if possible) should be sent to the server. 
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. Check for critical problems:
 *      1.1. obtain battery power. Make sure its above device_status.h::MIN_BATTERY_POWER.
 *      1.2. ping the main server ("connection check ping") and awaits response.
 *      1.3. check all sensors are working (i.e., there're meaningful readings which make sense).
 *      - if any error in either of these, enqueue (led-pattern) LowBattery \ NetworkingProblem \ BatteryPowerReadingProblem \ LoadCellReadingProblem
 *          and enqueue to msg-queue informative message(s)
 * 2. Check for non-critical 
 *      2.1. ping the NTP server and awaits response. If any errors, enqueue to msg-queue an informing message
 * 3. Finally, log (see logging.h) all results (including specific battery level), both positive checks and negative, with datetime stamp
 * 4. Only if critical error(s) occured, enqueue in the events queue SendLogFile
 * 
 * Output:
 *  - None.
 * 
 * Display:
 *  - Display is LEDOnly, ComputerOnly or Both as defined before compilation.
 *  - If it's LEDOnly / Both, a LEDPattern of activation should be displayed.
 *  - If it's ComputerOnly / Both, the following msgs should be presented to the user:
 *      - Device status check begins
 *      - (list each check with its result. Highlight critical errors)
 *      - (if Critical error other than main server communication) Sending results to the main server <IP of the main server>
 *  - on deployment, device is always LEDOnly 
 * 
 * Errors:
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
*/
void onCheckDeviceStatus();

/** Load-cells calibration logic
 * Calibration's to be performed after every activation. user should know that when calibrating, no garbage should be on the device.
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. Ask server for the weighing plate weight
 *  2. Blink the leds for callibration (user should remove now everything that is on the weight)
 *  3. Wait for a button press (user approves they're ready for calibration)
 *  4. Calibrate (see logic in the code commented below the function decleration).
 *  5. Save calibration ratio to EEPROM
 * 
 * Output:
 *  - None.
 * 
 * Display:
 *  - None. Except for the blinking as described in step 2.
 * 
 * Errors:
 *  - if no valid weighing plate weight is obtained, log it and enqueue EventType::SendLogFile with priority 1
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 *  2. Right below the function decleration there's a code that may help implementing the logic
*/
void onCalibrateLoadCell();
/*
   This example shows how to calibrate the load cell and optionally store the calibration
   value in EEPROM, and also how to change the value manually.
   The result value can then later be included in your project sketch or fetched from EEPROM.
* /
/*
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <SPI.h>


// HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

constexpr int calVal_eepromAdress = 0;
unsigned long t = 0;

void calibrate()
{
  Serial.println("***");
  Serial.println("Start calibration:");
  Serial.println("Place the load cell an a level stable surface.");
  Serial.println("Remove any load applied to the load cell.");
  Serial.println("Send 't' from serial monitor to set the tare offset.");

  boolean resume = false;
  while (!resume)
  {
    LoadCell.update();
    if (Serial.available() > 0)
    {
      if (Serial.available() > 0)
      {
        char inByte = Serial.read();
        if (inByte == 't')
          LoadCell.tareNoDelay();
      }
    }
    if (LoadCell.getTareStatus() == true)
    {
      Serial.println("Tare complete");
      resume = true;
    }
  }

  Serial.println("Now, place your known mass on the loadcell.");
  Serial.println("Then send the weight of this mass in grams (i.e. 100.0) from the serial monitor.");

  float known_mass = 0;
  resume = false;
  while (resume == false)
  {
    LoadCell.update();
    if (Serial.available() > 0)
    {
      known_mass = Serial.parseFloat();
      if (known_mass != 0)
      {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        resume = true;
      }
    }
  }
  // refresh the dataset to be sure that the known mass is measured correct
  LoadCell.refreshDataSet();
  // get the new calibration value
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass);

  Serial.print("New calibration value has been set to: ");
  Serial.print(newCalibrationValue);
  Serial.println(", use this as calibration value (calFactor) in your project sketch.");

  Serial.print("Save this value to EEPROM adress ");
  Serial.print(calVal_eepromAdress);
  Serial.println("? y/n");

  resume = false;
  while (!resume)
  {
    if (Serial.available() > 0)
    {
      char inByte = Serial.read();
      if (inByte == 'y')
      {
        EEPROM.begin(512);
        EEPROM.put(calVal_eepromAdress, newCalibrationValue);
        EEPROM.commit();
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        Serial.print("Value ");
        Serial.print(newCalibrationValue);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(calVal_eepromAdress);
        resume = true;
      }
      else if (inByte == 'n')
      {
        Serial.println("Value not saved to EEPROM");
        resume = true;
      }
    }
  }

  Serial.println("End calibration");
  Serial.println("***");
  Serial.println("To re-calibrate, send 'r' from serial monitor.");
  Serial.println("For manual edit of the calibration value, send 'c' from serial monitor.");
  Serial.println("***");
}

void setup()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;                 // set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag())
  {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1)
      ;
  }
  else
  {
    LoadCell.setCalFactor(1.0); // user set calibration value (float), initial value 1.0 may be used for this sketch
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update())
    ;
  calibrate(); // start calibration procedure
}

void loop()
{
  static boolean newDataReady = false;
  constexpr int serialPrintInterval = 500; // increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update())
    newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady)
  {
    if (millis() > t + serialPrintInterval)
    {
      float i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      newDataReady = 0;
      t = millis();
    }
  }

  // receive command from serial terminal
  if (Serial.available() > 0)
  {
    char inByte = Serial.read();
    if (inByte == 't')
      LoadCell.tareNoDelay(); // tare
    else if (inByte == 'r')
      calibrate(); // calibrate
  }

  // check if last tare operation is complete
  if (LoadCell.getTareStatus() == true)
  {
    Serial.println("Tare complete");
  }
}

*/

/** Change-transmission-times logic
 * This is used by the server to regulate load, and spread it evenly along the day.
 * Tx is the abbreviation for transmit, Rx is for receive.
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. Device asks server for the tx times (2 in total, each time is of 24 hours format)
 *  2. it stores them in an array in the EEPROM
 *  3. it tells the scheduler to reschedul its events.
 * 
 * Output:
 *  - None.
 * 
 * Display:
 *  - None.
 * 
 * Errors:
 *  - communication error
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 */
void onChangeTxTimes();

/** Send log file logic
 * log files are sent to the server regularly, with the data, or when needed to to diagnose problems.
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. generate a checksum of the log file
 *  2. send the log file to the server.
 *  3. wait for receiving a checksum from the server.
 *  4. if they match send to the server that all is good. if not, log the error and resend the log file
 *  5. delete the existing logfile
 *  6. create a new logfile
 * 
 * Output:
 *  - None.
 * 
 * Display:
 *  - None. Except for errors
 * 
 * Errors:
 *  - if communication error, enqueue relevant led-pattern \ msg to queue
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 */
void onSendLogFile();

/** Send data table logic
 * data tables are sent to the server regularly.
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  1. generate a checksum of the data
 *  2. send the data to the server.
 *  3. wait for receiving a checksum from the server.
 *  4. if they match send to the server that all is good. if not, log the error and resend the data
 *  5. delete the existing data
 *  6. create a new data table
 * 
 * Output:
 *  - None.
 * 
 * Display:
 *  - None. Except for errors
 * 
 * Errors:
 *  - if communication error, enqueue relevant led-pattern \ msg to queue
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 */
void onSendData();

/** clock calibration logic
 * Calibration's to be performed after once a 24 hours.
 * 
 * Input:
 *  - None. If input is needed, you may add input parametrs.
 * 
 * Behaviour:
 *  - ESP32 should have some builtin functions to handle callibrating clock with NTP. it goes something like that:
 *  1. connect to NTP server
 *  2. ask for current time
 *  3. calibrate the inner-clock
 * 
 * Output:
 *  - None.
 * 
 * Display:
 *  - None.
 * 
 * Errors:
 *  - connection error, log it
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 *  2. Right below the function decleration there's a code that may help implementing the logic
*/
void onCalibrateClock();
