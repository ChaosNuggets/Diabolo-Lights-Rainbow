#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Diabolo_Light.h>

using namespace Diabolo_Light;

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

uint16_t hue = 0;

void setup() {
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    begin(1, [](){ hue = 0; });
}

void loop() {
    handle_button();

    for (int i = 0; i < NUM_LEDS; i++) {
        pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, 255));
    }
    pixels.show();

    hue += 5;
}
