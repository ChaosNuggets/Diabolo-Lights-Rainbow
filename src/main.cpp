#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Diabolo_Light.h>

using namespace Diabolo_Light;

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
uint16_t hue = 0;

void setup() {
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

    // Initialize the diabolo light with 1 on mode (there's a default off mode built into the library)
    // and make the board set the hue to 0 whenever the board turns on
    begin(1, [](){ hue = 0; });
}

void loop() {
    // Read button input and determine whether the mode should be changed.
    // Also determine whether or not the board should shut off.
    handle_button();

    // Do the rainbow effect
    for (int i = 0; i < NUM_LEDS; i++) {
        pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, 255));
    }
    pixels.show();
    hue += 5;
}
