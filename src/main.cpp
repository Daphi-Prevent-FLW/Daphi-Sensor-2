/**
 * This project is based on DominikVoigt's esp32-weightsensor: https://github.com/DominikVoigt/esp32-weightsensor/tree/main
 * which is licensed under MIT license.
 * Note that this project, unless otherwised noted, is licensed under GPL3
 * 
 * These are the comments of DominikVoigt: https://github.com/DominikVoigt
 *  Code skeleton for weight measurements acquired from:
 *     https://github.com/olkal/HX711_ADC/blob/master/examples/Read_1x_load_cell_interrupt_driven/Read_1x_load_cell_interrupt_driven.ino
 *   WiFi and MQTT code skeleton acquired from:
 *     https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
 */

/** TODOs
 * 1. create a display file to swithc between computer only, led only or both (the non-chosen should not be complied using if constexpr? [C++17])
 * 2. merge the calibration into the normal flow
 * 3. convert the U8g2lib-using code into computer display
 * 4. put the networkings in a different file
 * 5. put the EEPROM in a different file (It retains data when power's off. It's used for calibration data, wifi networking and passwords, etc)
 * 6. make a LED and a button for I/O: callibration, initail configs, etc. 
 */

#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <string>
#include <WiFi.h>
#include <PubSubClient.h>


/************************* CIRCUIT CONFIGS *************************/
// HX711 GPIO pins
constexpr int HX711_DOUT = 2; // MCU > HX711 dout pin, must be external interrupt capable!
constexpr int HX711_SCK = 3;  // MCU > HX711 sck pin

// Button GPIO pins
constexpr int BUTTON = 4;

// LED GPIO pins
constexpr int LED = 5;

// HX711 constructor:
HX711_ADC LoadCell(HX711_DOUT, HX711_SCK);
float calibrationValue = 743.72;
constexpr int calVal_eepromAdress = 0;
bool useEEPROM = true;
bool serviceEnabled = false;

unsigned long t = 0;
constexpr int serialPrintInterval = 100;
volatile boolean newDataReady;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
bool enableBootDisplay = true;

// Replace the next variables with your SSID/Password combination
//constexpr char *ssid = "FRITZ!Box 7530 BS";
//constexpr char *password = "09324416513504437202";

// Replace the variable with your IP, port, sensorID 
//IPAddress server(192, 168, 0, 118);
//constexpr int port = 1900;
//constexpr std::string sensorID = "1111";

// Weight displayed on the display
int displayedWeight = 0;
// Weight Measured in the previous iteration
int lastWeight = 0;
bool firstMeasurement = true;

// TODO: What do these do?
WiFiClient espClient;
PubSubClient client(espClient);


void displaySensorIDBottom()
{
  u8g2.setCursor(30, 60);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.print("Sensor ID: ");
  u8g2.print(sensorID.c_str());
}

void displayReconnectMessage() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 15);
  u8g2.print("Disconnected");
  u8g2.setCursor(0, 40);
  u8g2.print("Reconnecting...");
  displaySensorIDBottom();
  u8g2.sendBuffer();
}

// interrupt routine:
void dataReadyISR()
{
  if (LoadCell.update())
  {
    newDataReady = 1;
  }
}

void setupLoadcell()
{
  if (useEEPROM)
  {
    EEPROM.begin(512);
    EEPROM.get(calVal_eepromAdress, calibrationValue);
  }
  Serial.print("Using Calibration value:");
  Serial.println(calibrationValue);

  LoadCell.begin();
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag())
  {
    Serial.println("Timeout, check wiring to HX711 and pin designations");
    while (true)
      ;
  }
  else
  {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Startup is complete");
  }
  attachInterrupt(digitalPinToInterrupt(HX711_DOUT), dataReadyISR, FALLING);
}

void setupDisplay()
{
  u8g2.begin();
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
  // For now we do not care about any topics, so we do not implement it
}

void setupMQTT()
{
  setup_wifi();
  client.setServer(server, 1900);
  client.setCallback(callback);
}

void displayBoot()
{
  u8g2.setCursor(20, 45);
  u8g2.setFont(u8g2_font_ncenB24_tr);
  u8g2.print("TUM");
  u8g2.sendBuffer();
  sleep(1);
}

void setup()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  setupLoadcell();
  setupDisplay();
  if (enableBootDisplay)
  {
    displayBoot();
  }
  if (serviceEnabled) {
    setupMQTT();
  }
}

void displayWeight()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(0, 15);
  u8g2.print("Current Weight:");
  u8g2.setCursor(0, 40);
  u8g2.print(displayedWeight);
  u8g2.print(" grams");
  displaySensorIDBottom();
  u8g2.sendBuffer();
}

int gearState = 0;
void displayMeasuring()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(0, 15);
  u8g2.print("Measuring");
  u8g2.setCursor(0, 40);
  u8g2.print("Weight  ");

  switch (gearState)
  {
  case 0:
    u8g2.print("/");
    break;
  case 1:
    u8g2.print("-");
    break;
  case 2:
    u8g2.print("\\");
    break;
  }
  gearState = ++gearState % 3;
  displaySensorIDBottom();
  u8g2.sendBuffer();
}

void publishWeightToMQTT()
{
  std::string topic = "cocktail/weight/sensor_";
  topic = topic + sensorID;
  client.publish(topic.c_str(), reinterpret_cast<uint8_t *>(&displayedWeight), sizeof(float));
}

void handleNewWeightData()
{
  newDataReady = 0;
  // Cutoff milligrams as the weighing setup is not that precise
  int newWeight = static_cast<int>(LoadCell.getData());
  // The delta in weight is not large, do not update the display (could be due to noise)
  bool smallDelta = abs(newWeight - displayedWeight) <= 2;
  // The delta in weight is pretty large and thus the weight measurement has not yet stabilized, do not update the display
  bool largeDelta = abs(newWeight - lastWeight) >= 3;
  if (!firstMeasurement)
  {
    if (largeDelta)
    {
      displayMeasuring();
    }
    lastWeight = newWeight;
    if (smallDelta || largeDelta)
    {
      // Do not update the screen
      return;
    }
  }
  else
  {
    firstMeasurement = false;
    lastWeight = newWeight;
  }
  if (newWeight <= 2)
  {
    // for very small values or negative values just display 0 as the precision of the scale is not that great
    newWeight = 0;
  }
  displayedWeight = newWeight;
  if (serviceEnabled)
  {
    publishWeightToMQTT();
  }
  displayWeight();
  Serial.print("Load_cell output val: ");
  Serial.println(newWeight);
}

void reconnect()
{
  displayReconnectMessage();
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
      // display weight after reconnection
      displayWeight();
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
  if (serviceEnabled)
  {
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();
  }
  // get smoothed value from the dataset:
  if (newDataReady)
  {
    if (millis() > t + serialPrintInterval)
    {
      handleNewWeightData();
      t = millis();
    }
  }

  if (Serial.available() > 0)
  {
    char inByte = Serial.read();
    if (inByte == 't')
      LoadCell.tareNoDelay();
  }

  if (LoadCell.getTareStatus())
  {
    Serial.println("Tare complete");
  }
}


/************************* CALLIBRATION FUNCTIONS *************************/
/*
   This example file shows how to calibrate the load cell and optionally store the calibration
   value in EEPROM, and also how to change the value manually.
   The result value can then later be included in your project sketch or fetched from EEPROM.
*/
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