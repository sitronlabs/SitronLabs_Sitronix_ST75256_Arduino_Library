/* Arduino libraries */
#include <Arduino.h>
#include <Wire.h>
#include <st75256.h>

/* C/C++ libraries */
#include <stdint.h>

/* Peripherals */
#define DISPLAY_RESET_PIN 5
static st75256 m_display(192, 96);
static uint8_t m_buffer[192 * (96 / 8)];

void setup() {

    /* Setup i2c */
    Wire.begin();

    /* Setup display */
   if (m_display.setup(Wire, 0x3C, DISPLAY_RESET_PIN, m_buffer) < 0) {
        Serial.println(" [e] Failed to setup display!");
        while(1){}
    }
    if (m_display.detect() != true) {
        Serial.println(" [e] Failed to detect display!");
        while(1){}
    }

    /* Display something */
    m_display.clear();
    m_display.setTextSize(1);
    m_display.println("Hello");
    m_display.println("world");
    m_display.display();
}

void loop() {

}
