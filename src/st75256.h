#ifndef ST75256_H
#define ST75256_H

/* Arduino libraries */
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

/* C/C++ libraries */
#include <errno.h>
#include <stdint.h>

/**
 *
 */
class st75256 : public Adafruit_GFX {

   public:
    /* Setup */
    st75256(int width, int height) : Adafruit_GFX(width, height), m_active_width(width), m_active_height(height) {}
    int setup(TwoWire& i2c_library, const uint8_t i2c_address, const int pin_res, uint8_t* const buffer);
    // int setup(TwoWire& i2c_library, const uint8_t i2c_address, const int pin_res);
    bool detect(void);
    int brightness_set(const float ratio);
    int inverted_set(const bool inverted);
    void invertDisplay(bool i);

    /* Pixel manipulation */
    int clear(void);
    int pixel_set(const size_t x, const size_t y, const uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    /* Output */
    int display(void);

    /* Commands
     * Full list at section 9.1 page 59 */
    enum command {

        /* Command for changing extension */
        COMMAND_EXTENSION_1 = 0x30,
        COMMAND_EXTENSION_2 = 0x31,
        COMMAND_EXTENSION_3 = 0x38,
        COMMAND_EXTENSION_4 = 0x39,

        /* Commandset for extension 1 (starts at section 9.2.2 page 63) */
        COMMAND_DISPLAY_OFF = 0xAE,
        COMMAND_DISPLAY_ON = 0xAF,
        COMMAND_SCAN_DIRECTION = 0xBC,
        COMMAND_SLEEP_OUT = 0x94,
        COMMAND_SLEEP_IN = 0x95,

        /* Commandset for extension 2 (starts at section 9.2.31 page 77) */
    };
    int command_send(const uint8_t command);
    int data_send(const uint8_t data);
    int data_send(uint8_t* const data, const size_t length);

   protected:
    const size_t m_gdram_width = 256, m_gdram_height = 168;  //!<
    size_t m_active_width, m_active_height;                  //!<
    size_t m_blanking_h;                                     //!< Number of empty pixel columns, starting from the left.
    TwoWire* m_i2c_library = NULL;
    uint8_t m_i2c_address = 0;
    uint8_t* m_buffer = NULL;

    enum interface {
        INTERFACE_NONE,
        INTERFACE_I2C_LIGHT,
        INTERFACE_I2C_BUFFERED,
    } m_interface = INTERFACE_NONE;
    int m_rotation_handle(const size_t x, const size_t y, size_t& x_panel, size_t& y_panel) const;
};

#endif
