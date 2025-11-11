#pragma once
#include "types.h"


void initDisplay(DisplayMode displayMode);
void displayOnComputer(const char *msg);
void blinkLed(LEDPattern ledPattern);

/** Process responsible for displaying outputs to the user
 * Input:
 *  - const char *msg: the message to be presented to the user (if DisplayMode == ComputerOnly or DisplayMode == Both)
 *  - LEDPattern ledPattern: a blinking pattern (if DisplayMode == LEDOnly or DisplayMode == Both)
 * 
 * Behaviour:
 *  1. Initiate the requested display mode(s). Implement initDisplay. Display mode will NOT change during runtime.
 *  2. Dequeue the displayQueue (if empty, msg = "" and ledPattern = LEDPattern(LEDPatternType::None))
 *  3. Display the text message \ light the led according to the pattern
 * 
 * Output:
 *  - void: No output
 * 
 * Notes:
 *  1. You may add more constants, functions, classes, etc. as needed.
 *  2. This process isn't on the main process (aka, void loop) to make sure messages and blinkings will be displayed when needed with no delays.
 *  3. It's prefered that the blinking logic is designed modularly, so blinking patterns are changeable easely in the future (i.e., patterns on future differnet outputs, different leds, changing the pattern, etc)
 */
void display();