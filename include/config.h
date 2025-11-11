#pragma once
#include <cstdint>
#include "types.h"

/******  Constants  ******/
constexpr uint16_t senseInterval = 60 * 1000;   // 60 seconds

constexpr uint8_t EVENTS_QUEUE_LENGTH   = 10;
constexpr uint8_t NETWORK_QUEUE_LENGTH  = 10;
constexpr uint8_t DISPLAY_QUEUE_LENGTH  = 10;

constexpr uint8_t NUMBER_OF_TX_TIMES = 2;

constexpr uint8_t MAX_NUMBER_OF_WIFI_NETWORKS = 10;
constexpr uint8_t MAX_DISPLAY_MSG_LEN = 100;
constexpr uint8_t MAX_SSID_LEN = 32; // according to some websites, 32 is the max length of SSID
constexpr uint8_t MAX_WIFI_PASSWORD_LEN = 63; // according to some websites, 63 is the max length of password

constexpr gpio HX711_DOUT       = 2;
constexpr gpio HX711_SCK        = 3;
constexpr gpio LED              = 4;
constexpr gpio BUTTON           = 5;    // make sure it's deepsleep wakeup enabaled
constexpr gpio BATTERY_POWER    = 0;    // change 0 to a valid number


/******  Varialbes  ******/
/** All should be stored in a non-voltile memory */
uint8_t MxResendsToSrv = 5;                 // the maximum allowed number of trials to send data to server before it tries no more
TimeType TxTimes[NUMBER_OF_TX_TIMES] = { 0 * 60, 12 * 60 };   // transmission (Tx) times to server in minutes
WifiNetwork wifiNetwroks[MAX_NUMBER_OF_WIFI_NETWORKS];
