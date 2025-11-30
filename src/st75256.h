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
 * @brief ST75256 LCD display driver class
 *
 * This class provides an interface to the Sitronix ST75256 LCD controller. The device
 * can communicate over I2C and provides functionality to:
 * - Display graphics and text using Adafruit GFX library
 * - Control display brightness and inversion
 * - Manage pixel-level drawing operations
 *
 * @note Call setup() before using any other functions
 * @note Make sure to initialize the I2C library (Wire.begin()) before calling setup()
 */
class st75256 : public Adafruit_GFX {

   public:
    //!@{
    //! Initialization and setup
    /**
     * @brief Constructor for the ST75256 display driver
     *
     * Initializes the display with the specified width and height. The display
     * supports a maximum GDRAM size of 256x168 pixels, but can be configured
     * for smaller active display areas.
     *
     * @param[in] width Display width in pixels
     * @param[in] height Display height in pixels
     */
    st75256(int width, int height) : Adafruit_GFX(width, height), m_active_width(width), m_active_height(height) {}

    /**
     * @brief Configures the driver for I2C communication with buffered mode
     *
     * Sets up the ST75256 to communicate over I2C using a local buffer for
     * display data. The buffer allows efficient pixel manipulation before
     * sending data to the display.
     *
     * @param[in] i2c_library Reference to the TwoWire I2C library instance (typically Wire)
     * @param[in] i2c_address I2C address of the device (0x3C, 0x3D, 0x3E, or 0x3F)
     * @param[in] pin_res GPIO pin number connected to the reset (RES) pin
     * @param[in] buffer Pointer to the buffer that will be used to store a local copy
     *                   of the GDRAM, should be (m_active_width * ((m_active_height + 7) / 8)) bytes
     * @return 0 on success, or a negative error code otherwise
     * @note Call this from the Arduino setup() function
     * @note Make sure the I2C library has been initialized with Wire.begin()
     */
    int setup(TwoWire& i2c_library, const uint8_t i2c_address, const int pin_res, uint8_t* const buffer);
    // int setup(TwoWire& i2c_library, const uint8_t i2c_address, const int pin_res);
    //!@}

    //!@{
    //! Device detection
    /**
     * @brief Detects if the ST75256 device is present
     *
     * Verifies device presence by attempting to communicate over I2C. For I2C
     * interfaces, this checks if the device acknowledges its address.
     *
     * @return true if device is detected, false otherwise
     */
    bool detect(void);
    //!@}

    //!@{
    //! Display configuration
    /**
     * @brief Sets the display brightness
     *
     * Configures the display contrast/brightness level. The ratio parameter
     * should be between 0.0 (dim) and 1.0 (bright).
     *
     * @param[in] ratio Brightness ratio from 0.0 to 1.0
     * @return 0 on success, or a negative error code otherwise
     * @note Currently returns -1 (not implemented)
     */
    int brightness_set(const float ratio);

    /**
     * @brief Sets the display inversion state
     *
     * Controls whether the display shows inverted colors (white on black vs
     * black on white).
     *
     * @param[in] inverted true to invert display, false for normal display
     * @return 0 on success, or a negative error code otherwise
     * @note Currently returns -1 (not implemented)
     */
    int inverted_set(const bool inverted);

    /**
     * @brief Inverts the display (Adafruit GFX compatibility)
     *
     * Adafruit GFX library compatibility function that calls inverted_set().
     *
     * @param[in] i true to invert display, false for normal display
     */
    void invertDisplay(bool i);
    //!@}

    //!@{
    //! Pixel manipulation
    /**
     * @brief Clears the entire display
     *
     * Sets all pixels to off (black). For buffered interfaces, this clears
     * the local buffer. For unbuffered interfaces, this directly clears the
     * GDRAM.
     *
     * @return 0 on success, or a negative error code otherwise
     */
    int clear(void);

    /**
     * @brief Sets a single pixel to the specified color
     *
     * Modifies a pixel at the given coordinates. The color parameter is treated
     * as a boolean: non-zero values turn the pixel on (white), zero turns it off (black).
     * Coordinates are automatically rotated based on the current display rotation setting.
     *
     * @param[in] x X coordinate of the pixel
     * @param[in] y Y coordinate of the pixel
     * @param[in] color Pixel color (0 for off, non-zero for on)
     * @return 0 on success, or a negative error code otherwise
     */
    int pixel_set(const size_t x, const size_t y, const uint16_t color);

    /**
     * @brief Draws a pixel (Adafruit GFX compatibility)
     *
     * Adafruit GFX library compatibility function that calls pixel_set().
     *
     * @param[in] x X coordinate of the pixel
     * @param[in] y Y coordinate of the pixel
     * @param[in] color Pixel color (0 for off, non-zero for on)
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    //!@}

    //!@{
    //! Display output
    /**
     * @brief Updates the display with buffered data
     *
     * Sends the contents of the local buffer to the display GDRAM. This function
     * should be called after making changes to the buffer to update the visible display.
     * For unbuffered interfaces, this function does nothing.
     *
     * @return 0 on success, or a negative error code otherwise
     */
    int display(void);
    //!@}

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
    //!@{
    //! Low-level communication
    /**
     * @brief Sends a command byte to the display
     *
     * Low-level function to send a single command byte to the ST75256 controller.
     * Commands are used to configure display settings and control display behavior.
     *
     * @param[in] command Command byte to send
     * @return 0 on success, or a negative error code otherwise
     * @see enum command for available commands
     */
    int command_send(const uint8_t command);

    /**
     * @brief Sends a single data byte to the display
     *
     * Low-level function to send a single data byte to the display GDRAM.
     * Data bytes represent pixel values in the display memory.
     *
     * @param[in] data Data byte to send
     * @return 0 on success, or a negative error code otherwise
     */
    int data_send(const uint8_t data);

    /**
     * @brief Sends multiple data bytes to the display
     *
     * Low-level function to send multiple consecutive data bytes to the display GDRAM.
     * This is more efficient than calling data_send() multiple times for bulk transfers.
     *
     * @param[in] data Pointer to the data array to send
     * @param[in] length Number of bytes to send
     * @return 0 on success, or a negative error code otherwise
     */
    int data_send(uint8_t* const data, const size_t length);
    //!@}

   protected:
    const size_t m_gdram_width = 256, m_gdram_height = 168;  //!< Maximum GDRAM dimensions (256x168 pixels)
    size_t m_active_width, m_active_height;                  //!< Active display area dimensions
    size_t m_blanking_h;                                     //!< Number of empty pixel columns, starting from the left
    TwoWire* m_i2c_library = NULL;                           //!< Pointer to I2C library instance
    uint8_t m_i2c_address = 0;                               //!< Device I2C address
    uint8_t* m_buffer = NULL;                                //!< Pointer to local display buffer

    enum interface {
        INTERFACE_NONE,              //!< No interface configured
        INTERFACE_I2C_LIGHT,         //!< I2C interface without local buffer
        INTERFACE_I2C_BUFFERED,      //!< I2C interface with local buffer
    } m_interface = INTERFACE_NONE;  //!< Current communication interface

    /**
     * @brief Handles coordinate rotation and validation
     *
     * Converts logical coordinates (x, y) to panel coordinates (x_panel, y_panel)
     * based on the current display rotation setting. Also validates that the
     * coordinates are within the active display area.
     *
     * @param[in] x Logical X coordinate
     * @param[in] y Logical Y coordinate
     * @param[out] x_panel Output parameter for panel X coordinate
     * @param[out] y_panel Output parameter for panel Y coordinate
     * @return 0 on success, or a negative error code if coordinates are invalid
     */
    int m_rotation_handle(const size_t x, const size_t y, size_t& x_panel, size_t& y_panel) const;
};

#endif
