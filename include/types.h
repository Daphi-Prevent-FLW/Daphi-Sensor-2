#pragma once
#include <cstdint>
#include "config.h"

using gpio = uint8_t;               // GPIO pin number. GPIO stands for General-Purpose Input/Output
using TimeType = uint16_t;          // time in minutes. ranges from 0 (= 00:00) to 1439 (= 23:59). see data.h
using weightType = uint16_t;        // weight in integer grams. ranges from 0 to 65,535 (= 65.535 kg). see data.h
using dateType = uint32_t;

enum class EventType        : uint8_t { Setup, Activate, Deactivate, CheckDeviceStatus, CalibrateLoadCell, ChangeConfigs, SendLogFile, SendData, CalibrateClock };
enum class DisplayMode      : uint8_t { ComputerOnly, LEDOnly, Both };
enum class LEDPatternType   : uint8_t {
    None,                   // No light. Used when not called or when nothing to display
    
    // Running events
    RunningSetup,           // 2.5sec gap, 0.5sec blink - repeated
    RunningActivation,      // 3 * Morse code for A (Activation). I.e., [. -] * 3
    CheckingDeviceStatus,   // Morse code for T (sTatus)         [-]
    RunningCalibration,     // Morse code for K (Kalibration)    [- . -]

    // IO patterns
    ButtonPressed,          // After input is identified. Morse code for I (Input) [. .]
    ProcessOut,             // Morse code for O: Out            [- - -]

    // Indication patterns
    Good,                   // 3 * Morse code for E: Excellent  [.] * 3.
    MajorError              // 3sec constant light, no gap
};
enum class NetworkTask      : uint8_t { ConnectToWifi, ConnectToMainServer, SyncWithNTPServer, ListenToMainServer, SendMsgToMainServer };
enum class PwrConsumptionMode : uint8_t { DeepSleep, LightSleep, ModemSleep, Active };

struct Event {
    EventType eventType;
    uint8_t priority;
};

struct LEDPattern {
    LEDPatternType ledPatternType;
    static constexpr uint8_t priority = 0;  // Required by Queue, though unnecessary for LEDPattern. `static constexpr` ensures all LEDPattern::priority = 0 at compile time, with zero runtime overhead.

    // Constructor
    LEDPattern(LEDPatternType ledPatternType) : ledPatternType(ledPatternType) {}
};

struct Record { // for data.h
    TimeType recordTime;
    weightType weight;
};

struct DisplayTask { //for display.h
    const char msg[MAX_DISPLAY_MSG_LEN];
    LEDPattern ledPattern;
};

struct WifiNetwork {
    char ssid[MAX_SSID_LEN];
    char password[MAX_WIFI_PASSWORD_LEN];   // note that this is the length of an unhashed password. if it should be hashed, parameters of either type or length should be modified
};
