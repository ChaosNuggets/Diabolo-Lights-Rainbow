#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>

const int BUTTON_PIN = 2;
const int LED_PIN = 1;
const int MOSFET_PIN = 0;
const int NUM_LEDS = 6;

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

bool are_leds_on = false;
uint16_t hue = 0;

unsigned long last_debounce_time = 0;
const int DEBOUNCE_DELAY = 50; //ms
int debounce_button_state;
int button_state = HIGH; // Set it to high so then the second if block inside handle_button runs on startup if the button is low

ISR(PCINT0_vect) { // This should only run when it is woken up from sleep mode
    //for(int i = 0; i < NUM_LEDS; i++) {
    //    pixels.setPixelColor(i, pixels.Color(0, 50, 0));
    //    pixels.show();
    //}
    //delay(500);
    //pixels.clear();
    //pixels.show();

    digitalWrite(MOSFET_PIN, HIGH); // Connect the LEDs
    hue = 0; // Reset the color
    last_debounce_time = millis();
    are_leds_on = true;
    button_state = HIGH;

    cli(); // disable interrupts
    PCMSK &= ~(1 << PCINT2); // turns of PCINT2 as interrupt pin
    sleep_disable(); // clear sleep enable bit
}

static void shut_down() {
    //for(int i = 0; i < NUM_LEDS; i++) {
    //    pixels.setPixelColor(i, pixels.Color(50, 0, 0));
    //    pixels.show();
    //}
    //delay(500);
    //pixels.clear();
    //pixels.show();

    digitalWrite(MOSFET_PIN, LOW); // Disconnect the LEDs
    digitalWrite(LED_PIN, HIGH); // Make it so then ESD protection doesn't get in the way of reducing power
    GIMSK |= 1 << PCIE; // enable pin change interrupt
    PCMSK |= 1 << PCINT2; // turns on PCINT2 as interrupt pin
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable(); //set sleep enable bit
    sei(); // enable interrupts
    sleep_mode(); // put the microcontroller to sleep
}

static void handle_button() {
    int reading = digitalRead(BUTTON_PIN);
    if (reading != debounce_button_state) {
        last_debounce_time = millis();
        debounce_button_state = reading;
    }

    if ((millis() - last_debounce_time) > DEBOUNCE_DELAY && reading != button_state) {
        button_state = reading;

        if (are_leds_on && button_state == HIGH) {
            are_leds_on = false;
        } 

        if (!are_leds_on && button_state == LOW) {
            shut_down();
        }
    }
}

void setup() {
    ADCSRA &= ~(1 << ADEN); // Disable ADC
    ACSR &= ~(1 << ACIE); // Disable analog comparator interrupt
    ACSR |= 1 << ACD; // Disable analog comparator

    pinMode(MOSFET_PIN, OUTPUT);

    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

    pinMode(BUTTON_PIN, INPUT);
    debounce_button_state = digitalRead(BUTTON_PIN);
}

void loop() {
    handle_button();

    if (!are_leds_on) {
        pixels.clear();
        pixels.show();
        return;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, 255));
    }
    pixels.show();

    hue += 5;
}
