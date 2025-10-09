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


#include "config.h"         // some constants and configurations
#include "device_status.h"  // flags about device status and their function
#include "display.h"        // handles the display task
#include "events.h"         // handles events
#include "sensors.h"        // handles data from the event
#include "types.h"          // project-specific types and structs
#include "queue.h"          // queue class
#include "data.h"           // functions to handle the sensor data table
#include "scheduler.h"      // functions to handle scheduling tasks, like sending data to server
#include "networkings.h"    // functions to handle networking tasks

#include "freertos/FreeRTOS.h"  // esp32 built-in: multitasking header
#include "freertos/task.h"      // esp32 built-in: multitasking header


Queue<Event> eventsQueue(EVENTS_QUEUE_LENGTH);
/* Create messages queue and led-patterns queue */

void setup() {
    /* Initiate pinMode() here */

    xTaskCreate(getLoadCellData); // on "sensors.h"
    xTaskCreate(display);         // on "display.h"
    xTaskCreate(scheduler);       // on "scheduler.h"
    xTaskCreate(networkings);     // on "networkings.h"
}

void loop() {
    listenToEvents();
    while (!eventsQueue.isEmpty()) {
        Event event = eventsQueue.dequeue();
        switch (event.eventType) {
            case EventType::Setup: onSetup(); break;    // - If already activated, device should be de activated for setup, and then (re)activeted. so events queue should be (first to dequeued: ) ... deactivte, setup, activate, ... (last)
            case EventType::Activate: onActivate(); break;
            case EventType::Deactivate: onDeactivate(); break;
            case EventType::CheckDeviceStatus: onCheckDeviceStatus(); break;
            case EventType::CalibrateLoadCell: onCalibrateLoadCell(); break;
            case EventType::ChangeTxTimes: onChangeTxTimes(); break;
            case EventType::SendLogFile: onSendLogFile(); break;    // if no device status is logged, or if a device status is logged with errors, then first call onCheckDeviceStatus, then call onSendLogFile
            case EventType::SendData: onSendData(); break; // should immidietly enqueue EventType::SendLogFile with priority 1 (highest)
            case EventType::CalibrateClock: onCalibrateClock(); break;
        }
    }
}

/* Old Code to take snippets from
 
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <string>
#include <WiFi.h>
#include <PubSubClient.h>


/************************* CIRCUIT CONFIGS ************************* /
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

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=* /U8X8_PIN_NONE);
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


*/